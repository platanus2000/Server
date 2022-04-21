//
// Created by yu on 12/7/2021.
//
#include "MYSQLConnection.h"
#include "QueryResult.h"
#include "DatabaseWorkerPool.h"
#include "DatabaseWorkerPool.cpp"
#include "AdhocStatement.h"
#include "DatabaseWorker.h"

using namespace std;

int main()
{
//	MySQLConnectionInfo info("127.0.0.1;3306;root;123456@Ab;Phoenix");
//	MySQLConnection conn(info);
//	conn.Open();
//	string createSql = "CREATE TABLE `test_user`( \
//						`user_id` bigint(20) UNSIGNED NOT NULL DEFAULT '0', \
//						`passward` varchar(255) NOT NULL DEFAULT '')";
//	//conn.Execute(createSql.c_str());
//
//	for (size_t i = 0; i < 1; i++)
//	{
//		string querySql = "SELECT account, user_id from role";
//		ResultSet* res = conn.Query(querySql.c_str());
//		LOG_INFO("rowCount: %lu", res->GetRowCount());
//		do
//		{
//			Field* field = res->Fetch();
//			LOG_INFO("account: %s, uid: %lu", field[0].GetString().c_str(), field[1].GetUInt64());
//		} while (res->NextRow());
//	}
//	while(true)
//	{
//		usleep(10000);
//	}

	string querySql = "SELECT account, user_id from role";
	DatabaseWorkerPool<MySQLConnection>* pool = new DatabaseWorkerPool<MySQLConnection>();
	pool->SetConnectionInfo("10.246.88.175;3306;root;123456@Ab;Phoenix", 2, 3);
	pool->Open();
	auto cb = pool->AsyncQuery(querySql.c_str()).WithCallback([](QueryResult result){
		do
		{
			Field* field = result->Fetch();
			LOG_INFO("account: %s, uid: %lu", field[0].GetString().c_str(), field[1].GetUInt64());
		} while (result->NextRow());
	});

	while (true)
	{
		cb.InvokeIfReady();
	}

	return 0;
}
