//
// Created by yu on 12/10/2021.
//

#include "DatabaseWorkerPool.h"
#include "ProducerConsumerQueue.h"
#include "mysql/mysql.h"
#include "MYSQLConnection.h"
#include "AdhocStatement.h"
#include "QueryResult.h"

class PingOperation : public SQLOperation
{
	//! Operation for idle delaythreads
	bool Execute() override
	{
		m_conn->Ping();
		return true;
	}
};

template <class T>
DatabaseWorkerPool<T>::DatabaseWorkerPool() :
_queue(new ProducerConsumerQueue<SQLOperation*>()),
_async_threads(0), _synch_threads(0)
{
	assert(mysql_thread_safe());
}

template <class T>
DatabaseWorkerPool<T>::~DatabaseWorkerPool()
{
	_queue->Cancel();
}

template <class T>
void DatabaseWorkerPool<T>::SetConnectionInfo(std::string const& infoString, uint8 const asyncThreads, uint8 const synchThreads)
{
	_connectionInfo = std::make_unique<MySQLConnectionInfo>(infoString);

	_async_threads = asyncThreads;
	_synch_threads = synchThreads;
}

template <class T>
uint32 DatabaseWorkerPool<T>::Open()
{
	assert(_connectionInfo.get());
	LOG_INFO("Opening DatabasePool '%s'. Asynchronous connections: %u, synchronous connections: %u.",
		GetDatabaseName(), _async_threads, _synch_threads);

	uint32 error = OpenConnections(IDX_ASYNC, _async_threads);

	if (error)
		return error;

	error = OpenConnections(IDX_SYNCH, _synch_threads);

	if (!error)
	{
		LOG_INFO("DatabasePool opened successfully.");
	}

	return error;
}

template <class T>
void DatabaseWorkerPool<T>::Close()
{
	LOG_INFO("Closing down DatabasePool '%s'.", GetDatabaseName());

	//! Closes the actualy MySQL connection.
	_connections[IDX_ASYNC].clear();

	LOG_INFO("Asynchronous connections on DatabasePool '%s' terminated. "
							  "Proceeding with synchronous connections.",
		GetDatabaseName());

	//! Shut down the synchronous connections
	//! There's no need for locking the connection, because DatabaseWorkerPool<>::Close
	//! should only be called after any other thread tasks in the core have exited,
	//! meaning there can be no concurrent access at this point.
	_connections[IDX_SYNCH].clear();

	LOG_INFO("All connections on DatabasePool '%s' closed.", GetDatabaseName());
}

template <class T>
void DatabaseWorkerPool<T>::Execute(char const* sql)
{
	BasicStatementTask* task = new BasicStatementTask(sql);
	Enqueue(task);
}

template <class T>
void DatabaseWorkerPool<T>::DirectExecute(char const* sql)
{
	T* connection = GetFreeConnection();
	connection->Execute(sql);
	connection->Unlock();
}

template <class T>
QueryResult DatabaseWorkerPool<T>::Query(char const* sql, T* connection /*= nullptr*/)
{
	if (!connection)
		connection = GetFreeConnection();

	ResultSet* result = connection->Query(sql);
	connection->Unlock();
	if (!result || !result->GetRowCount() || !result->NextRow())
	{
		delete result;
		return QueryResult(nullptr);
	}

	return QueryResult(result);
}

template <class T>
QueryCallback DatabaseWorkerPool<T>::AsyncQuery(char const* sql)
{
	BasicStatementTask* task = new BasicStatementTask(sql, true);
	// Store future result before enqueueing - task might get already processed and deleted before returning from this method
	std::future<QueryResult> result = task->GetFuture();
	Enqueue(task);
	return QueryCallback(std::move(result));
}

template <class T>
uint32 DatabaseWorkerPool<T>::OpenConnections(InternalIndex type, uint8 numConnections)
{
	for (uint8 i = 0; i < numConnections; ++i)
	{
		// Create the connection
		auto connection = [&] {
		  switch (type)
		  {
		  case IDX_ASYNC:
			  return std::make_unique<T>(_queue.get(), *_connectionInfo);
		  case IDX_SYNCH:
			  return std::make_unique<T>(*_connectionInfo);
		  default:
			  abort();
		  }
		}();

		if (uint32 error = connection->Open())
		{
			// Failed to open a connection or invalid version, abort and cleanup
			_connections[type].clear();
			return error;
		}
//		else if (connection->GetServerVersion() < MIN_MYSQL_SERVER_VERSION)
//		{
//			LOG_FATAL("TrinityCore does not support MySQL versions below " MIN_MYSQL_SERVER_VERSION_STRING " (found id %u, need id >= %u), please update your MySQL server", connection->GetServerVersion(), MIN_MYSQL_SERVER_VERSION);
//			return 1;
//		}
		else
		{
			_connections[type].push_back(std::move(connection));
		}
	}

	// Everything is fine
	return 0;
}

template <class T>
void DatabaseWorkerPool<T>::KeepAlive()
{
	//! Ping synchronous connections
	for (auto& connection : _connections[IDX_SYNCH])
	{
		if (connection->LockIfReady())
		{
			connection->Ping();
			connection->Unlock();
		}
	}

	//! Assuming all worker threads are free, every worker thread will receive 1 ping operation request
	//! If one or more worker threads are busy, the ping operations will not be split evenly, but this doesn't matter
	//! as the sole purpose is to prevent connections from idling.
	auto const count = _connections[IDX_ASYNC].size();
	for (uint8 i = 0; i < count; ++i)
		Enqueue(new PingOperation);
}

template <class T>
void DatabaseWorkerPool<T>::EscapeString(std::string& str)
{
	if (str.empty())
		return;

	char* buf = new char[str.size() * 2 + 1];
	EscapeString(buf, str.c_str(), uint32(str.size()));
	str = buf;
	delete[] buf;
}

template <class T>
unsigned long DatabaseWorkerPool<T>::EscapeString(char* to, char const* from, unsigned long length)
{
	if (!to || !from || !length)
		return 0;

	return _connections[IDX_SYNCH].front()->EscapeString(to, from, length);
}

template <class T>
char const* DatabaseWorkerPool<T>::GetDatabaseName() const
{
	return _connectionInfo->database.c_str();
}

template <class T>
void DatabaseWorkerPool<T>::Enqueue(SQLOperation* op)
{
	_queue->Push(op);
}

template <class T>
size_t DatabaseWorkerPool<T>::QueueSize() const
{
	return _queue->Size();
}

template <class T>
T* DatabaseWorkerPool<T>::GetFreeConnection()
{
	uint8 i = 0;
	auto const num_cons = _connections[IDX_SYNCH].size();
	T* connection = nullptr;
	//! Block forever until a connection is free
	for (;;)
	{
		connection = _connections[IDX_SYNCH][++i % num_cons].get();
		//! Must be matched with t->Unlock() or you will get deadlocks
		if (connection->LockIfReady())
			break;
	}

	return connection;
}