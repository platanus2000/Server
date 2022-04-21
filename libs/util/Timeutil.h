#pragma once
#include "common.h"
#include <iomanip>

namespace timeutil 
{
	typedef uint64 Time;  // milliseconds from 1971

	inline Time AddSeconds(Time timeValue, int second) 
    {
		return timeValue + second * 1000;
	}

	inline Time AddMilliseconds(Time timeValue, int milliseconds) 
    {
		return timeValue + milliseconds;
	}

	inline std::string TimestampToStr(Time tampTime, char *pStr)
	{
		//auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm* stLocal = localtime((time_t*)&tampTime);		//转为北京时间  
		strftime(pStr, 64, "%Y-%m-%d %H:%M:%S", stLocal);	//转为字符串时间
		std::stringstream ss;
		ss << std::put_time(std::localtime((time_t*)&tampTime), "%F %T");	//%F %T 等价于 %Y-%m-%d %H:%M:%S。 更加细粒度的控制，可以自己对应组合（%Y，%m...分别对应年月日时分秒，怎么控制输出随便你）。比如现在我只需要打印 时-分 这样的格式，换成 %H - %M 即可
		return ss.str();
	}
}