#include "LoggerWindows.h"
#include<iostream>
LoggerWindows::LoggerWindows()
{
    const char* logName = "WatchDog";

    // Create and save the handle to register windows logs
    m_eventHandle = RegisterEventSourceA(NULL, logName);
}

void LoggerWindows::log(const std::string& log, int logLevel)
{
    auto logCharArray = log.c_str();

    // Report an event, currently only the log message is sent
    ReportEventA(m_eventHandle, logLevel, 0, 0, NULL, 1, 0, &logCharArray, NULL);
}
