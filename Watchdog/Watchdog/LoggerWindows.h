#pragma once
#include<Windows.h>

#include "LoggerInterface.h"

class LoggerWindows : public LoggerInterface
{
	HANDLE m_eventHandle;
public:
	LoggerWindows();
	void log(const std::string& log, int logLevel) override;

	//Removed copy, move and assignment constructor for the class and the default constructor is private
private:
	LoggerWindows(LoggerWindows& processMonitor) = delete;
	LoggerWindows(LoggerWindows&& processMonitor) = delete;
	LoggerWindows& operator=(const LoggerWindows& t) = delete;
};

