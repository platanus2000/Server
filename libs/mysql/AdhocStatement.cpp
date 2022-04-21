//
// Created by yu on 12/10/2021.
//

#include "AdhocStatement.h"
#include "QueryResult.h"
#include "MYSQLConnection.h"

BasicStatementTask::BasicStatementTask(char const* sql, bool async) :
	m_result(nullptr)
{
	m_sql = strdup(sql);
	m_has_result = async; // If the operation is async, then there's a result
	if (async)
		m_result = new std::promise<QueryResult>;
}

BasicStatementTask::~BasicStatementTask()
{
	free((void*)m_sql);
	if (m_has_result && m_result != nullptr)
		delete m_result;
}

bool BasicStatementTask::Execute()
{
	if (m_has_result)
	{
		ResultSet* result = m_conn->Query(m_sql);
		if (!result || !result->GetRowCount() || !result->NextRow())
		{
			delete result;
			m_result->set_value(QueryResult(nullptr));
			return false;
		}
		m_result->set_value(QueryResult(result));
		return true;
	}
	return m_conn->Execute(m_sql);
}

