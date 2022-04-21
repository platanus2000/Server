//
// Created by yu on 12/10/2021.
//
#include "MYSQLConnection.h"

#pragma once

class TestDatabase : public MySQLConnection
{
public:
	//- Constructors for sync and async connections
	TestDatabase(MySQLConnectionInfo& connInfo);
	TestDatabase(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo);
	~TestDatabase();
};