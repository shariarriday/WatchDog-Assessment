#pragma once

#include<vector>
#include<memory>
#include<string>
#include<future>

#include "ProcessHelperWindows.h"
#include "Observer.h"
#include "LoggerInterface.h"

class JSONParser;

class ProcessMonitor : public Observer
{
	std::vector<Process> m_processHandles;
	std::shared_ptr<JSONParser> m_jsonParser;
	std::shared_ptr<ProcessHelperInterface> m_processHelper;
	std::shared_ptr<LoggerInterface> m_logger;
	std::future<void> m_currentTask;

	bool m_watch;
public:
	ProcessMonitor(std::shared_ptr<JSONParser> jsonParser, std::shared_ptr<LoggerInterface> logger);

	//Removed copy, move and assignment constructor for the class
	ProcessMonitor(ProcessMonitor& processMonitor) = delete;
	ProcessMonitor(ProcessMonitor&& processMonitor) = delete;
	ProcessMonitor& operator=(const ProcessMonitor& t) = delete;

	void monitorProcess();
	void update() override;
	void stopWatch();

private:
	std::map<std::string, HANDLE> getProcessList();
	void setupMonitor(std::map<std::string, HANDLE>& runningProcessList, const std::vector<ProcessDetails>& monitorList);
	void cleanup(const std::map<std::string, HANDLE>& openProcesses);
};

