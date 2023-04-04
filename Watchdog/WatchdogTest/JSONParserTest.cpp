#include "pch.h"

#include<memory>
#include<fstream>
#include<filesystem>
#include "JSONParser.h"
#include "LoggerWindows.h"


using json = nlohmann::json;

class JSONParserTest : public ::testing::Test {
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
    }

    void TearDown() override {
        std::filesystem::remove("testData.json");
    }
public:

    std::shared_ptr<JSONParser> jsonParser;
};

TEST_F(JSONParserTest, IsEmptyInitially) {
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 0);
}

TEST_F(JSONParserTest, IsParsedCorrectly) {
    jsonParser->parseProcess();
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 1);
}

TEST_F(JSONParserTest, ValuesAreParsedCorrectly) {
    jsonParser->parseProcess();
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails[0].processCommand, "");
    EXPECT_EQ(initialDetails[0].processName, "Notepad.exe");
    EXPECT_EQ(initialDetails[0].processPath, "Notepad.exe");
}

TEST_F(JSONParserTest, IsUpdatedCorrectly) {

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
    Sleep(50);
    jsonParser->update();
    auto initialDetails = jsonParser->getProcessDetails();
    EXPECT_EQ(initialDetails.size(), 2);
}