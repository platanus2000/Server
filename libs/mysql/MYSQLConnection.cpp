#include "MYSQLConnection.h"
#include "Util.h"
#include "Logger.h"
#include "QueryResult.h"
#include "DatabaseWorker.h"
#include <mysql/errmsg.h>

MySQLConnectionInfo::MySQLConnectionInfo(std::string const& infoString)
{
	std::vector<std::string_view> tokens = Util::Tokenize(infoString, ';', true);

	if (tokens.size() != 5 && tokens.size() != 6)
		return;

	host.assign(tokens[0]);
	port_or_socket.assign(tokens[1]);
	user.assign(tokens[2]);
	password.assign(tokens[3]);
	database.assign(tokens[4]);

	if (tokens.size() == 6)
		ssl.assign(tokens[5]);
}

MySQLConnection::MySQLConnection(MySQLConnectionInfo& connInfo) :
m_connectionFlags(CONNECTION_SYNCH),
m_mysql(nullptr),
m_connectionInfo(connInfo),
m_queue(nullptr)
{
}

MySQLConnection::MySQLConnection(ProducerConsumerQueue<SQLOperation*>* queue, MySQLConnectionInfo& connInfo) :
	m_connectionFlags(CONNECTION_ASYNC),
	m_mysql(nullptr),
	m_connectionInfo(connInfo),
	m_queue(queue),
	m_worker(new DatabaseWorker(m_queue, this))
{
}

MySQLConnection::~MySQLConnection()
{
	Close();
}

uint32 MySQLConnection::Open()
{
	MYSQL* mysqlInit = mysql_init(nullptr);
	if (mysqlInit == nullptr)
	{
		LOG_FATAL("mysql_init error");
		return CR_UNKNOWN_ERROR;
	}

	mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8");

	m_mysql = mysql_real_connect(mysqlInit, m_connectionInfo.host.c_str(), m_connectionInfo.user.c_str(), m_connectionInfo.password.c_str(), m_connectionInfo.database.c_str(), atoi(m_connectionInfo.port_or_socket.c_str()), "", 0);
	if (m_mysql == nullptr)
	{
		LOG_FATAL("Could not connect to MySQL database at %s: %s", m_connectionInfo.database.c_str(), mysql_error(mysqlInit));
		uint32 errorCode = mysql_errno(mysqlInit);
		mysql_close(mysqlInit);
		return errorCode;
	}

	LOG_INFO("Connected to MySQL database at %s", m_connectionInfo.host.c_str());
	mysql_autocommit(m_mysql, 1);
	// set connection properties to UTF8 to properly handle locales for different
	// server configs - core sends data in UTF8, so MySQL must expect UTF8 too
	mysql_set_character_set(m_mysql, "utf8");
	return 0;
}

void MySQLConnection::Close()
{
	m_worker.reset();
	if (m_mysql)
	{
		mysql_close(m_mysql);
		m_mysql = nullptr;
	}
}

bool MySQLConnection::Execute(const char* sql)
{
	if (!m_mysql)
		return false;

	uint32 _s = Util::getMSTime();

	if (mysql_query(m_mysql, sql))
	{
		uint32 lErrno = mysql_errno(m_mysql);
		LOG_INFO("SQL: %s", sql);
		LOG_FATAL("[%u] %s", lErrno, mysql_error(m_mysql));
		return false;
	}
	else
	{
		LOG_INFO("[%u ms] SQL: %s", Util::getMSTimeDiff(_s, Util::getMSTime()), sql);
	}
	return true;
}

ResultSet* MySQLConnection::Query(const char* sql)
{
	if (!sql)
		return nullptr;

	MYSQL_RES* result = nullptr;
	MYSQL_FIELD* fields = nullptr;
	uint64 rowCount = 0;
	uint32 fieldCount = 0;

	if (!_Query(sql, &result, &fields, &rowCount, &fieldCount))
		return nullptr;

	return new ResultSet(result, fields, rowCount, fieldCount);
}

bool MySQLConnection::_Query(const char* sql, MYSQL_RES** pResult, MYSQL_FIELD** pFields, uint64* pRowCount, uint32* pFieldCount)
{
	if (!m_mysql)
		return false;

	uint32 _s = Util::getMSTime();
	if (mysql_query(m_mysql, sql))
	{
		uint32 lErrno = mysql_errno(m_mysql);
		LOG_INFO("SQL: %s", sql);
		LOG_FATAL("[%u] %s", lErrno, mysql_error(m_mysql));
		return false;
	}
	else
	{
		LOG_INFO("[%u ms] SQL: %s", Util::getMSTimeDiff(_s, Util::getMSTime()), sql);
	}

	*pResult = mysql_store_result(m_mysql);
	*pRowCount = mysql_affected_rows(m_mysql);		//返回前一次 MySQL 操作所影响的记录行数
	*pFieldCount = mysql_field_count(m_mysql);		//返回作用在连接上的最近查询的列数

	if (!*pResult)
		return false;
	if (!*pRowCount)
	{
		mysql_free_result(*pResult);
		return false;
	}
	*pFields = mysql_fetch_field(*pResult);			//返回采用MYSQL_FIELD结构的结果集的列

	return true;
}

size_t MySQLConnection::EscapeString(char* to, const char* from, size_t length)
{
	return mysql_real_escape_string(m_mysql, to, from, length);
}

void MySQLConnection::Ping()
{
	mysql_ping(m_mysql);
}

bool MySQLConnection::LockIfReady()
{
	return m_Mutex.try_lock();
}

void MySQLConnection::Unlock()
{
	m_Mutex.unlock();
}
