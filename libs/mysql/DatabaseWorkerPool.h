//
// Created by yu on 12/10/2021.
//

#pragma once
#include "common.h"
#include "QueryCallback.h"

template <typename T>
class ProducerConsumerQueue;

class SQLOperation;
struct MySQLConnectionInfo;

template <class T>
class DatabaseWorkerPool
{
private:
	enum InternalIndex
	{
		IDX_ASYNC,
		IDX_SYNCH,
		IDX_SIZE
	};

public:
	DatabaseWorkerPool();

	~DatabaseWorkerPool();

	void SetConnectionInfo(std::string const& infoString, uint8 const asyncThreads, uint8 const synchThreads);

	uint32 Open();

	void Close();

	inline MySQLConnectionInfo const* GetConnectionInfo() const { return _connectionInfo.get(); }

	//! Enqueues a one-way SQL operation in string format that will be executed asynchronously.
	//! This method should only be used for queries that are only executed once, e.g during startup.
	void Execute(char const* sql);

	//! Directly executes a one-way SQL operation in string format, that will block the calling thread until finished.
	//! This method should only be used for queries that are only executed once, e.g during startup.
	void DirectExecute(char const* sql);

	//! Directly executes an SQL query in string format that will block the calling thread until finished.
	//! Returns reference counted auto pointer, no need for manual memory management in upper level code.
	QueryResult Query(char const* sql, T* connection = nullptr);

	//! Enqueues a query in string format that will set the value of the QueryResultFuture return object as soon as the query is executed.
	//! The return value is then processed in ProcessQueryCallback methods.
	QueryCallback AsyncQuery(char const* sql);

	size_t QueueSize() const;

	//! Keeps all our MySQL connections alive, prevent the server from disconnecting us.
	void KeepAlive();

	//! Apply escape string'ing for current collation. (utf8)
	void EscapeString(std::string& str);
private:
	uint32 OpenConnections(InternalIndex type, uint8 numConnections);

	char const* GetDatabaseName() const;
	void Enqueue(SQLOperation* op);

	unsigned long EscapeString(char* to, char const* from, unsigned long length);

	//从同步的连接池中取一个
	T* GetFreeConnection();
private:
	std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> _queue;
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> _connections;
	std::unique_ptr<MySQLConnectionInfo> _connectionInfo;
	std::vector<uint8> _preparedStatementSize;
	uint8 _async_threads, _synch_threads;
};
