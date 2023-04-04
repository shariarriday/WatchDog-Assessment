#include "FileWatcher.h"
#include<Windows.h>
#include<filesystem>
#include<chrono>
#include<iostream>

FileWatcher::FileWatcher(std::string jsonPath) : 
	m_filePath(jsonPath),
	m_watch(true)
{
}

void FileWatcher::addFileObserver(std::shared_ptr<Observer> observer)
{
	m_fileObservers.push_back(observer);
}

void FileWatcher::addProcessObserver(std::shared_ptr<Observer> observer)
{
	m_processObservers.push_back(observer);
}

void FileWatcher::watchFile()
{
	// Checks if the file exists
	if (!std::filesystem::exists(m_filePath)) {
		return;
	}

	// Initial update to the observers
	runUpdater();

	auto currentTime = std::filesystem::_File_time_clock::now();

	// Check the file in an infinite loop and compare the last write time to the time to the currentTime and update currentTime to last write time when 
	// a file is updated
	while (m_watch) {
		auto fileWriteTime = std::filesystem::last_write_time(m_filePath);
		auto distance = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch() - fileWriteTime.time_since_epoch()).count();
		if (distance < 0) {
			std::cout << "Detected new write" << distance << std::endl;
			currentTime = std::filesystem::_File_time_clock::now();
			Sleep(100); // This is added to ensure that the file is saved properly before access, Runtime error is caused without this sleep
			runUpdater();
		}
	}
}

void FileWatcher::stopWatch()
{
	m_watch = false;
}

void FileWatcher::runUpdater()
{
	for (const auto& observer : m_fileObservers) {
		observer->update();
	}
	for (const auto& observer : m_processObservers) {
		observer->update();
	}
}