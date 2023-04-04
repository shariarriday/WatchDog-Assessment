#pragma once

#include "json.hpp"
#include "Observer.h"
#include "LoggerInterface.h"

// The details for the process that will be used by other classes
struct ProcessDetails{
	std::string processName;
	std::string processCommand;
	std::string processPath;
};

class JSONParser : public Observer
{
	using json = nlohmann::json;

	std::string m_jsonPath;
	json m_jsonObject;

	std::vector<ProcessDetails> m_processDetails;
	std::shared_ptr<LoggerInterface> m_logger;

public:
	JSONParser(std::string jsonPath, std::shared_ptr<LoggerInterface> logger);

	//Removed copy, move and assignment constructor for the class
	JSONParser(JSONParser& jsonParser) = delete;
	JSONParser(JSONParser&& jsonParser) = delete;
	JSONParser& operator=(const JSONParser& t) = delete;

	void update() override;

	void parseProcess();
	std::vector<ProcessDetails> getProcessDetails();
};

