#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "Logger.h"
#include <future>
#include <queue>

#pragma pack(4)
struct MsgHeader
{
   uint16_t msgID = 0;
   uint32_t dataLen = 0;
};
#pragma pack()

#define HEAD_SIZE sizeof(MsgHeader)

#define INVALID_SOCKET 0

//#ifdef EPOLL
   #define SOCKET int
//#endif

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using int32 = int32_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using int64 = int64_t;
using int64 = int64_t;
using uint64 = uint64_t;
using uint64 = uint64_t;

#define MAX_CLIENT  5120

static const int32 kNoneEvent = 0;
static const int32 kReadEvent = EPOLLIN | EPOLLPRI;
static const int32 kWriteEvent = EPOLLOUT;

class ConnectObj;
class Buffer;
using ConnectObjPtr = std::shared_ptr<ConnectObj>;
using BufferPtr = std::shared_ptr<Buffer>;
using MessageCallBack = std::function<void(const ConnectObjPtr&, const BufferPtr&, const MsgHeader*)>;

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

template<typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From>& f) 
{
   if (false)
   {
      implicit_cast<From*, To*>(0);
   }

#ifndef NDEBUG
   assert(f == NULL || dynamic_cast<To*>(get_pointer(f)) != NULL);
#endif
   return ::std::static_pointer_cast<To>(f);
}

class ResultSet;
using QueryResult = std::shared_ptr<ResultSet>;
using QueryResultFuture = std::future<QueryResult>;
using QueryResultPromise = std::promise<QueryResult>;

