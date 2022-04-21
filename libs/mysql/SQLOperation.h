#pragma once
#include "nocopyable.h"

//- Union that holds element data
union SQLElementUnion
{
    //PreparedStatementBase* stmt;
    char const* query;
};

//- Type specifier of our element data
enum SQLElementDataType
{
    SQL_ELEMENT_RAW,
    SQL_ELEMENT_PREPARED
};

//- The element
struct SQLElementData
{
    SQLElementUnion element;
    SQLElementDataType type;
};

class MySQLConnection;

class SQLOperation : public nocopyable
{
public:
    SQLOperation() : m_conn(nullptr) { }
    virtual ~SQLOperation() { }

    virtual int call()
    {
		Execute();
        return 0;
    }

    virtual bool Execute() = 0;
    virtual void SetConnection(MySQLConnection* con) { m_conn = con; }

	MySQLConnection* m_conn;
};