#pragma once

#include<string>
#include<vector>
#include<memory>

#include "Observer.h"

class FileWatcher
{
	std::string m_filePath;
	std::vector<std::shared_ptr<Observer>> m_fileObservers;
	std::vector<std::shared_ptr<Observer>> m_processObservers;

	bool m_watch;
public:
	FileWatcher(std::string jsonPath);

	//Removed copy, move and assignment constructor for the class
	FileWatcher(FileWatcher& jsonParser) = delete;
	FileWatcher(FileWatcher&& jsonParser) = delete;
	FileWatcher& operator=(const FileWatcher& t) = delete;

	void addProcessObserver(std::shared_ptr<Observer> observer);
	void addFileObserver(std::shared_ptr<Observer> observer);
	void watchFile();
	void stopWatch();

private:
	void runUpdater();
};

