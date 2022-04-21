//
// Created by yu on 12/10/2021.
//

#pragma once
#include "SQLOperation.h"
#include "common.h"

/*! Raw, ad-hoc query. */
class BasicStatementTask : public SQLOperation
{
public:
	BasicStatementTask(char const* sql, bool async = false);
	~BasicStatementTask();

	bool Execute() override;
	QueryResultFuture GetFuture() const { return m_result->get_future(); }

private:
	char const* m_sql;      //- Raw query to be executed
	bool m_has_result;
	std::promise<QueryResult>* m_result;
};