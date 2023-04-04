#include <iostream>

#include "ProcessMonitor.h"
#include "ProcessHelperInterface.h"
#include "JSONParser.h"

ProcessMonitor::ProcessMonitor(std::shared_ptr<JSONParser> jsonParser, std::shared_ptr<LoggerInterface> logger) :
	m_jsonParser(jsonParser),
	m_processHelper(std::make_shared<ProcessHelperWindows>(logger)),
	m_logger(logger),
	m_watch(true)
{
}

void ProcessMonitor::monitorProcess()
{
	auto monitorList = m_jsonParser->getProcessDetails();
	auto runningProcessList = getProcessList();
	
	m_processHandles.clear();

	setupMonitor(runningProcessList, monitorList);

	// Watch if any process needs to be restarted in an infinite loop. This loop can be shut down with the m_watch flag from a different thread
	while (m_watch && m_processHandles.size() > 0) {
		auto restartHandle = m_processHelper->monitor(m_processHandles);
		if (restartHandle >= 0) {
			auto handler = m_processHelper->open(monitorList.at(restartHandle).processPath, monitorList.at(restartHandle).processCommand);
			if (handler.has_value()) {
				m_logger->log("Process Restarted", SUCCESS);
				auto newProcessEntry = Process({ handler.value(), monitorList.at(restartHandle).processName});
				m_processHandles[restartHandle] = newProcessEntry;
			}
			else {
				m_logger->log("Process Not Restarted", WARNING);
			}
		}
	}

	cleanup(runningProcessList);
}

void ProcessMonitor::update()
{
	// File is modified, shut down previous thread and run a new one
	m_logger->log("Process monitor task restarted", SUCCESS);

	m_watch = false;
	m_processHelper->setWatch(false);

	if (m_currentTask.valid()) {
		m_currentTask.get();
	}

	m_watch = true;
	m_processHelper->setWatch(true);
	m_currentTask = std::async(std::launch::async, [this] { monitorProcess(); });
}

void ProcessMonitor::stopWatch()
{
	if (!m_currentTask.valid()) {
		return;
	}

	m_watch = false;
	m_processHelper->setWatch(false);
	m_currentTask.get();
}

std::map<std::string, HANDLE> ProcessMonitor::getProcessList()
{
	auto processList = m_processHelper->enumerate();
	
	return processList;
}

void ProcessMonitor::setupMonitor(std::map<std::string, HANDLE>& runningProcessList, const std::vector<ProcessDetails>& monitorList)
{
	// populate the m_processHandles with the proper values
	for (const auto& monitorProcess : monitorList) {
		if (runningProcessList.find(monitorProcess.processName) == runningProcessList.end()) {
			auto handler = m_processHelper->open(monitorProcess.processPath, monitorProcess.processCommand);
			if (handler.has_value()) {
				auto newProcessEntry = Process({ handler.value(), monitorProcess.processName });
				m_processHandles.push_back(std::move(newProcessEntry));
			}
		}
		else {
			bool found = false;
			for (int i = 0; i < m_processHandles.size(); i++) {
				if (m_processHandles.at(i).name == monitorProcess.processName) {
					m_processHandles[i].handle = runningProcessList[monitorProcess.processName];
					found = true;
				}
			}
			if (!found) {
				auto newProcessEntry = Process({ runningProcessList[monitorProcess.processName], monitorProcess.processName });
				m_processHandles.push_back(std::move(newProcessEntry));
			}
		}
	}
}

void ProcessMonitor::cleanup(const std::map<std::string, HANDLE>& openProcesses)
{
	// Cleanup for win32 handles
	for (const auto& process : openProcesses) {
		m_processHelper->clean(process.second);
	}
}