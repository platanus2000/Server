/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "QueryCallback.h"

template<typename T, typename... Args>
inline void Construct(T& t, Args&&... args)
{
	new (&t) T(std::forward<Args>(args)...);
}

template<typename T>
inline void Destroy(T& t)
{
	t.~T();
}

template<typename T>
inline void ConstructActiveMember(T* obj)
{
	Construct(obj->_string);
}

template<typename T>
inline void DestroyActiveMember(T* obj)
{
	Destroy(obj->_string);
}

template<typename T>
inline void MoveFrom(T* to, T&& from)
{
	to->_string = std::move(from._string);
}

struct QueryCallback::QueryCallbackData : public nocopyable
{
public:
	friend class QueryCallback;

	QueryCallbackData(std::function<void(QueryCallback&, QueryResult)>&& callback) : _string(std::move(callback)) { }
	//QueryCallbackData(std::function<void(QueryCallback&, PreparedQueryResult)>&& callback) : _prepared(std::move(callback)), _isPrepared(true) { }
	QueryCallbackData(QueryCallbackData&& right)
	{
		ConstructActiveMember(this);
		MoveFrom(this, std::move(right));
	}
	QueryCallbackData& operator=(QueryCallbackData&& right)
	{
		if (this != &right)
		{
			MoveFrom(this, std::move(right));
		}
		return *this;
	}
	~QueryCallbackData() { DestroyActiveMember(this); }

private:
	template<typename T> friend void ConstructActiveMember(T* obj);
	template<typename T> friend void DestroyActiveMember(T* obj);
	template<typename T> friend void MoveFrom(T* to, T&& from);

	union
	{
		std::function<void(QueryCallback&, QueryResult)> _string;
		//std::function<void(QueryCallback&, PreparedQueryResult)> _prepared;
	};
};

// Not using initialization lists to work around segmentation faults when compiling with clang without precompiled headers
QueryCallback::QueryCallback(QueryResultFuture&& result)
{
	//_string = std::move(result);
	Construct(_string, std::move(result));
	//Construct 等价于 new (&_string) std::future<QueryResult>(std::forward<std::future<QueryResult>>(std::move(result)));
	/*
	 * T (&t) new T() T value 在由 &t 指定的地址中构造. "放置"new
	 * 	_string = result; 因为future的=是被删除的。。。。
	 * */
}

QueryCallback::QueryCallback(QueryCallback&& right)
{
	ConstructActiveMember(this);
	MoveFrom(this, std::move(right));
	_callbacks = std::move(right._callbacks);
}

QueryCallback& QueryCallback::operator=(QueryCallback&& right)
{
	if (this != &right)
	{
		MoveFrom(this, std::move(right));
		_callbacks = std::move(right._callbacks);
	}
	return *this;
}

QueryCallback::~QueryCallback()
{
	DestroyActiveMember(this);
}

QueryCallback&& QueryCallback::WithCallback(std::function<void(QueryResult)>&& callback)
{
	return WithChainingCallback([callback](QueryCallback& /*this*/, QueryResult result) { callback(std::move(result)); });
}

QueryCallback&& QueryCallback::WithChainingCallback(std::function<void(QueryCallback&, QueryResult)>&& callback)
{
	//assert(!_callbacks.empty());
	_callbacks.emplace(std::move(callback));
	return std::move(*this);
}

void QueryCallback::SetNextQuery(QueryCallback&& next)
{
	MoveFrom(this, std::move(next));
}

bool QueryCallback::InvokeIfReady()
{
	QueryCallbackData& callback = _callbacks.front();
	auto checkStateAndReturnCompletion = [this]()
	{
	  _callbacks.pop();
	  bool hasNext = _string.valid();
	  if (_callbacks.empty())
	  {
		  assert(!hasNext);
		  return true;
	  }

	  // abort chain
	  if (!hasNext)
		  return true;

	  return false;
	};

	if (_string.valid() && _string.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		std::future<QueryResult> f(std::move(_string));
		std::function<void(QueryCallback&, QueryResult)> cb(std::move(callback._string));
		cb(*this, f.get());
		return checkStateAndReturnCompletion();
	}

	return false;
}
