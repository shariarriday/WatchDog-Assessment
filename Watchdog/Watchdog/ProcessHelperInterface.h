#pragma once

#include<optional>
#include<string>
#include<vector>
#include<map>

#include "JSONParser.h"

#ifdef _WIN32
#include<Windows.h>
typedef HANDLE HANDLE;
#endif

struct Process {
	HANDLE handle;
	std::string name;
};

class ProcessHelperInterface
{
public:
	virtual std::map<std::string, HANDLE> enumerate() = 0;
	virtual std::optional<HANDLE> open(const std::string& path, const std::string& command) = 0;
	virtual int monitor(const std::vector<Process>& watchList) = 0;
	virtual void setWatch(bool watch) = 0;
	virtual void clean(HANDLE handle) = 0;
	virtual void close(const std::string& handle) = 0;
};

