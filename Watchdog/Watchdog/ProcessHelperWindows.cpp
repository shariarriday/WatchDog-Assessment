#include "ProcessHelperWindows.h"

#include <tchar.h>
#include <psapi.h>
#include <iostream>

ProcessHelperWindows::ProcessHelperWindows(std::shared_ptr<LoggerInterface> logger) :
    m_watch(true),
    m_logger(logger)
{
}

std::map<std::string, HANDLE> ProcessHelperWindows::enumerate()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    std::map<std::string, HANDLE> processList;
    
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return std::map<std::string, HANDLE>();
    }

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.
    for (unsigned int i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            auto process = getProcessByPID(aProcesses[i]);
            if (process.has_value()) {
                processList.insert({ process.value().name, process.value().handle });
            }
        }
    }

    return processList;
}

std::optional<HANDLE> ProcessHelperWindows::open(const std::string& path, const std::string& command)
{
    // Open a new process based on the provided parameters

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string fullCommand = path + " " + command;

    if (!CreateProcessA(NULL,
        const_cast<char *>(fullCommand.c_str()),        // Command line
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi)
        )
    {
        return std::nullopt;
    }

    return pi.hProcess;
}

int ProcessHelperWindows::monitor(const std::vector<Process>& watchList)
{
    // Create raw array pointer for win32 API and populate it
    HANDLE* handles = new HANDLE[watchList.size()];
    for (size_t i = 0; i < watchList.size(); i++) {
        handles[i] = watchList.at(i).handle;
    }

    // Monitor in a loop
    while (m_watch && watchList.size() > 0) {
        auto resultEvent = WaitForMultipleObjects(
            watchList.size(),
            handles,
            FALSE,
            1000);
        // Check if any process is stopped and restart it by returning it's index to the caller. Caller can use this index to identify the handle
        if (resultEvent >= WAIT_OBJECT_0 && resultEvent <= WAIT_OBJECT_0 + watchList.size()) {
            m_logger->log("Program stopped", 100);
            delete[] handles;
            return resultEvent - WAIT_OBJECT_0;
        }
        std::cout << "Monitoring" << std::endl;
    }

    // Cleanup before return
    delete[] handles;
    return -1;
}

void ProcessHelperWindows::setWatch(bool watch)
{
    m_watch = watch;
}

void ProcessHelperWindows::clean(HANDLE handle)
{
    CloseHandle(handle);
}

std::optional<Process> ProcessHelperWindows::getProcessByPID(DWORD pid)
{
    CHAR szProcessName[MAX_PATH] = "<unknown>";

    // Get a handle to the process.
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE | PROCESS_TERMINATE,
        FALSE, 
        pid);

    if (NULL != hProcess)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        // Enum process modules and get the module's base name
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(CHAR));

            return Process{ hProcess, std::string(szProcessName) };
        }
    }

    return std::nullopt;
}

void ProcessHelperWindows::close(const std::string& handle) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    std::map<std::string, HANDLE> processList;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return;
    }

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Close all process with the name
    for (unsigned int i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            auto process = getProcessByPID(aProcesses[i]);
            if (process.has_value() && process.value().name == handle) {
                TerminateProcess(process.value().handle, 0);
            }
        }
    }
}