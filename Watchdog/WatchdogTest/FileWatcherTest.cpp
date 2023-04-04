#include "pch.h"

#include<memory>
#include<fstream>
#include<filesystem>
#include<future>

#include "JSONParser.h"
#include "LoggerWindows.h"
#include "FileWatcher.h"

using json = nlohmann::json;

class FileWatcherTest : public ::testing::Test {
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
        fileWatcher->addFileObserver(jsonParser);
    }

    void TearDown() override {
        std::filesystem::remove("testData.json");
    }
public:

    std::shared_ptr<JSONParser> jsonParser;
    std::shared_ptr<FileWatcher> fileWatcher;
};

TEST_F(FileWatcherTest, IsEmptyInitially) {
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 0);
}

TEST_F(FileWatcherTest, IsParsedOnStartup) {
    auto task = std::async(std::launch::async, [this] { fileWatcher->watchFile(); });
    Sleep(1000); // Give time to setup properly;
    fileWatcher->stopWatch();
    task.get();
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 1);
}

TEST_F(FileWatcherTest, IsReparsedOnChange) {
    auto task = std::async(std::launch::async, [this] { fileWatcher->watchFile(); });
    Sleep(1000); // Give time to setup properly;
    json testData = json::parse(R"(
        [  
            {
                "name" : "Notepad.exe",
                "command" : "",
                "path" : "Notepad.exe"
            },
            {
                "name" : "Notepad2.exe",
                "command" : "Second Command",
                "path" : "Notepad2.exe"
            }
        ]
        )");
    std::ofstream save("testData.json");
    save << std::setw(4) << testData << std::endl;
    Sleep(1000);
    fileWatcher->stopWatch();
    task.get();
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 2);
}