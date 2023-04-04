#include "JSONParser.h"

#include <filesystem>
#include <fstream>
#include <iostream>

JSONParser::JSONParser(std::string jsonPath, std::shared_ptr<LoggerInterface> logger) :
	m_jsonPath(jsonPath),
	m_logger(logger)
{
}

void JSONParser::parseProcess()
{
	if (!std::filesystem::exists(m_jsonPath)) {
		// Invalid json file provided so we shut down the process
		std::terminate();
		return;
	}

	std::ifstream jsonFile(m_jsonPath);
	m_jsonObject = json::parse(jsonFile);

	m_processDetails.clear();

	// Iterate and save all of the processes in a vector
	for (auto& [key, value] : m_jsonObject.items()) {
		m_processDetails.push_back({ value["name"], value["command"], value["path"]});
	}
}

std::vector<ProcessDetails> JSONParser::getProcessDetails()
{
	return m_processDetails;
}

void JSONParser::update() 
{
	// Observer function, this will call the parseProcess function when the file is updated
	m_logger->log("JSON File updated", SUCCESS);
	parseProcess();
}