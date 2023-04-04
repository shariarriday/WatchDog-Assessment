#include "JSONParser.h"
#include "FileWatcher.h"
#include "ProcessMonitor.h"
#include "LoggerWindows.h"

#include<iostream>

int main()
{
    std::string path;
    std::cout << "Please provide a path for the json file: ";
    std::getline(std::cin, path);
    // Initialize the objects in the main function
    auto logger = std::make_shared<LoggerWindows>();
    auto jsonParser = std::make_shared<JSONParser>(path, logger);
    auto processMonitor = std::make_shared<ProcessMonitor>(jsonParser, logger);

    FileWatcher jsonWatcher(path);
    jsonWatcher.addFileObserver(jsonParser);
    jsonWatcher.addProcessObserver(processMonitor);

    // Monitor from a seperate thread
    auto watcher = std::async(std::launch::async, [&jsonWatcher]{ jsonWatcher.watchFile(); });
    
    // A loop can be written here to ensure that the async thread is shut down for safe termination.
    watcher.get();
    return 0;
}

