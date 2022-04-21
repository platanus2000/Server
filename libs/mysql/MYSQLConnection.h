#pragma once
#include "common.h"
#include <mysql/mysql.h>

class ResultSet;

enum ConnectionFlags
{
	CONNECTION_ASYNC = 0x1,
	CONNECTION_SYNCH = 0x2,
	CONNECTION_BOTH = CONNECTION_ASYNC | CONNECTION_SYNCH
};

struct MySQLConnectionInfo
{
    explicit MySQLConnectionInfo(std::string const& infoString);

    std::string user;
    std::string password;
    std::string database;
    std::string host;
    std::string port_or_socket;
    std::string ssl;
};

template <typename T>
class ProducerConsumerQueue;
class DatabaseWorker;
class SQLOperation;

class  MySQLConnection : public nocopyable
{
public:
    MySQLConnection(MySQLConnectionInfo& connInfo);
	MySQLConnection(ProducerConsumerQueue<SQLOperation*>* queue, MySQLConnectionInfo& connInfo);
    virtual ~MySQLConnection();

    virtual uint32 Open();
    void Close();

    bool Execute(const char* sql);

	ResultSet* Query(const char* sql);


	size_t EscapeString(char* to, const char* from, size_t length);

	void Ping();

protected:
	/// Tries to acquire lock. If lock is acquired by another thread
	/// the calling parent will just try another connection
	bool LockIfReady();

	/// Called by parent databasepool. Will let other threads access this connection
	void Unlock();

private:
	bool _Query(const char* sql, MYSQL_RES** pResult, MYSQL_FIELD** pFields, uint64* pRowCount, uint32* pFieldCount);
    
private:
	ConnectionFlags       m_connectionFlags;            //! Connection flags (for preparing relevant statements)
    MYSQL* m_mysql;
    MySQLConnectionInfo& m_connectionInfo;
	ProducerConsumerQueue<SQLOperation*>* m_queue;      //! Queue shared with other asynchronous connections.
	std::unique_ptr<DatabaseWorker> m_worker;           //! Core worker task.
	std::mutex m_Mutex;
};