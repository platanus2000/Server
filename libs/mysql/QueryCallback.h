//
// Created by yu on 12/10/2021.
//

#pragma once
#include "common.h"

class QueryCallback : public nocopyable
{
public:
	explicit QueryCallback(QueryResultFuture&& result);
	//explicit QueryCallback(PreparedQueryResultFuture&& result);
	QueryCallback(QueryCallback&& right);
	QueryCallback& operator=(QueryCallback&& right);
	~QueryCallback();

	QueryCallback&& WithCallback(std::function<void(QueryResult)>&& callback);
	//QueryCallback&& WithPreparedCallback(std::function<void(PreparedQueryResult)>&& callback);

	QueryCallback&& WithChainingCallback(std::function<void(QueryCallback&, QueryResult)>&& callback);
	//QueryCallback&& WithChainingPreparedCallback(std::function<void(QueryCallback&, PreparedQueryResult)>&& callback);

	// Moves std::future from next to this object
	void SetNextQuery(QueryCallback&& next);

	// returns true when completed
	bool InvokeIfReady();

private:
	template<typename T> friend void ConstructActiveMember(T* obj);
	template<typename T> friend void DestroyActiveMember(T* obj);
	template<typename T> friend void MoveFrom(T* to, T&& from);

	union
	{
		QueryResultFuture _string;
		//PreparedQueryResultFuture _prepared;
	};
	//bool _isPrepared;

	struct QueryCallbackData;
	std::queue<QueryCallbackData, std::list<QueryCallbackData>> _callbacks;
};
