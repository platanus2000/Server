#pragma once
#include "common.h"

namespace Util
{
    std::vector<std::string_view> Tokenize(std::string_view str, char sep, bool keepEmpty)
    {
        std::vector<std::string_view> tokens;
        size_t start = 0;
        for (size_t end = str.find(sep); end != std::string_view::npos; end = str.find(sep, start))
        {
            if (keepEmpty || (start < end))
                tokens.push_back(str.substr(start, end - start));
            start = end + 1;
        }

        if (keepEmpty || (start < str.length()))
            tokens.push_back(str.substr(start));

        return tokens;
    }

	inline std::chrono::steady_clock::time_point GetApplicationStartTime()
	{
		using namespace std::chrono;
		static const steady_clock::time_point ApplicationStartTime = steady_clock::now();
		return ApplicationStartTime;
	}

	inline uint32 getMSTime()
	{
		using namespace std::chrono;
		return uint32(duration_cast<milliseconds>(steady_clock::now() - GetApplicationStartTime()).count());
	}

	inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
	{
		// getMSTime() have limited data range and this is case when it overflow in this tick
		if (oldMSTime > newMSTime)
			return (0xFFFFFFFF - oldMSTime) + newMSTime;
		else
			return newMSTime - oldMSTime;
	}
}


