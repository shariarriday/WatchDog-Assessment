#pragma once
#include<string>

#ifdef _WIN32
#include<Windows.h>
#define SUCCESS EVENTLOG_SUCCESS
#define WARNING EVENTLOG_WARNING_TYPE
#endif

class LoggerInterface {
public:
	virtual void log(const std::string& log, int logLevel) = 0;
};