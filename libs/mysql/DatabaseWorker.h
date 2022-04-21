//
// Created by yu on 12/10/2021.
//

#pragma once
#include "common.h"
#include "atomic"

template <typename T>
class ProducerConsumerQueue;
class MySQLConnection;
class SQLOperation;

class DatabaseWorker
{
public:
	DatabaseWorker(ProducerConsumerQueue<SQLOperation*>* newQueue, MySQLConnection* connection);
	~DatabaseWorker();

private:
	ProducerConsumerQueue<SQLOperation*>* _queue;
	MySQLConnection* _connection;
	void WorkerThread();
	std::thread _workerThread;
	std::atomic<bool> _cancelationToken;
};