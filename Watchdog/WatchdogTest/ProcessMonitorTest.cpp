#include "pch.h"

#include<memory>
#include<fstream>
#include<filesystem>
#include<future>

#include "JSONParser.h"
#include "LoggerWindows.h"
#include "ProcessMonitor.h"
#include "ProcessHelperWindows.h"
#include "FileWatcher.h"

using json = nlohmann::json;

class ProcessMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        json testData = json::parse(R"(
        [  
            {
                "name" : "Notepad.exe",
                "command" : "",
                "path" : "Notepad.exe"
            }
        ]
        )");
        std::ofstream save("testData.json");
        save << std::setw(4) << testData << std::endl;
        jsonParser = std::make_shared<JSONParser>("testData.json", std::make_shared<LoggerWindows>());
        fileWatcher = std::make_shared<FileWatcher>("testData.json");
        processMonitor = std::make_shared<ProcessMonitor>(jsonParser, std::make_shared<LoggerWindows>());
        fileWatcher->addFileObserver(jsonParser);
        fileWatcher->addProcessObserver(processMonitor);
    }

    void TearDown() override {
        std::filesystem::remove("testData.json");
    }
public:

    std::shared_ptr<JSONParser> jsonParser;
    std::shared_ptr<FileWatcher> fileWatcher;
    std::shared_ptr<ProcessMonitor> processMonitor;
};

TEST_F(ProcessMonitorTest, IsEmptyInitially) {
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 0);
}

TEST_F(ProcessMonitorTest, StartsProcessAutomatically) {
    auto task = std::async(std::launch::async, [this] { fileWatcher->watchFile(); });
    Sleep(1000); // Give time to setup properly;
    std::shared_ptr<ProcessHelperInterface> processHelper = std::make_shared<ProcessHelperWindows>(std::make_shared<LoggerWindows>());
    auto openedProcess = processHelper->enumerate();
    EXPECT_NE(openedProcess.find("Notepad.exe"), openedProcess.end());
    fileWatcher->stopWatch();
    processMonitor->stopWatch();
    task.get();
    processHelper->close("Notepad.exe");
}

TEST_F(ProcessMonitorTest, RestartsProcesses) {
    auto task = std::async(std::launch::async, [this] { fileWatcher->watchFile(); });
    Sleep(1000); // Give time to setup properly;
    std::shared_ptr<ProcessHelperInterface> processHelper = std::make_shared<ProcessHelperWindows>(std::make_shared<LoggerWindows>());
    
    auto openedProcess = processHelper->enumerate();
    EXPECT_NE(openedProcess.find("Notepad.exe"), openedProcess.end());
    processHelper->close("Notepad.exe");
    
    openedProcess = processHelper->enumerate();
    EXPECT_NE(openedProcess.find("Notepad.exe"), openedProcess.end());

    fileWatcher->stopWatch();
    processMonitor->stopWatch();
    task.get();

    Sleep(1000);
    processHelper->close("Notepad.exe");

    Sleep(1000);
    openedProcess = processHelper->enumerate();
    EXPECT_EQ(openedProcess.find("Notepad.exe"), openedProcess.end());
}