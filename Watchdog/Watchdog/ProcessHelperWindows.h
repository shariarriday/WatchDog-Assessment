#pragma once
#include "ProcessHelperInterface.h"
#include "LoggerWindows.h"

class ProcessHelperWindows : public ProcessHelperInterface
{
	bool m_watch;
	std::shared_ptr<LoggerInterface> m_logger;

public:
	ProcessHelperWindows(std::shared_ptr<LoggerInterface> logger);
	
	std::map<std::string, HANDLE> enumerate() override;
	std::optional<HANDLE> open(const std::string& path, const std::string& command) override;
	int monitor(const std::vector<Process>& watchList) override;

	void setWatch(bool watch) override;
	void clean(HANDLE handle) override;
	void close(const std::string& handle) override;

//Removed copy, move and assignment constructor for the class and the default constructor is private
	ProcessHelperWindows(ProcessHelperWindows& processMonitor) = delete;
	ProcessHelperWindows(ProcessHelperWindows&& processMonitor) = delete;
	ProcessHelperWindows& operator=(const ProcessHelperWindows& t) = delete;

private:
	std::optional<Process> getProcessByPID(DWORD pid);
};

