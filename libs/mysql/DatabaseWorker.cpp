//
// Created by yu on 12/10/2021.
//

#include "DatabaseWorker.h"
#include "MYSQLConnection.h"
#include "SQLOperation.h"
#include "ProducerConsumerQueue.h"

DatabaseWorker::DatabaseWorker(ProducerConsumerQueue<SQLOperation*>* newQueue, MySQLConnection* connection)
{
	_connection = connection;
	_queue = newQueue;
	_cancelationToken = false;
	_workerThread = std::thread(&DatabaseWorker::WorkerThread, this);
}

DatabaseWorker::~DatabaseWorker()
{
	_cancelationToken = true;

	_queue->Cancel();

	_workerThread.join();
}

void DatabaseWorker::WorkerThread()
{
	if (!_queue)
		return;

	for (;;)
	{
		SQLOperation* operation = nullptr;

		_queue->WaitAndPop(operation);

		if (_cancelationToken || !operation)
			return;

		operation->SetConnection(_connection);
		operation->call();

		delete operation;
	}
}


