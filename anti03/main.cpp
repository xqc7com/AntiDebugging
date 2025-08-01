#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>

DWORD GetParentProcessId(DWORD nProcessId)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(PROCESSENTRY32);
    BOOL bRet = Process32First(hSnapshot, &pe32);
    while (bRet)
    {
        if (pe32.th32ProcessID == nProcessId)
        {
            CloseHandle(hSnapshot);
            return pe32.th32ParentProcessID;
        }
        bRet = Process32Next(hSnapshot, &pe32);
    }
    CloseHandle(hSnapshot);
    return 0;
}

std::string GetProcessNameByProcessId(DWORD nProcessId)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return std::string("");
    }

    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(PROCESSENTRY32);
    BOOL bRet = Process32First(hSnapshot, &pe32);
    while (bRet)
    {
        if (pe32.th32ProcessID == nProcessId)
        {
            std::string processName = pe32.szExeFile;
            CloseHandle(hSnapshot);
            return processName;
        }
        bRet = Process32Next(hSnapshot, &pe32);
    }
    CloseHandle(hSnapshot);
    return std::string("");
}

bool CheckParentProcess(const char* szProcessName)
{
    DWORD nProcessId = GetCurrentProcessId();
    DWORD nParentProcessId = GetParentProcessId(nProcessId);
    std::string processName = GetProcessNameByProcessId(nParentProcessId);
    std::cout << "parent:" << processName << std::endl;
    return (strstr(processName.c_str(), szProcessName) == NULL);
}

void ThreadProc()
{
    while (true)
    {
        if (CheckParentProcess("explorer"))
        {
            std::cout << "Debugging..." << std::endl;
        }
        else
        {
            std::cout << "Running..." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main()
{
    std::thread thrd(ThreadProc);
    thrd.join();
    return 0;
}