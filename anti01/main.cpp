#include <iostream>
#include <thread>
#include <windows.h>
#include <TlHelp32.h>

bool CheckProcess(const char* szProcessName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(PROCESSENTRY32);
    BOOL bRet = Process32First(hSnapshot, &pe32);
    while (bRet)
    {
        if (strstr(pe32.szExeFile, szProcessName) != NULL)
        {
            std::cout << "find debug process:" << pe32.szExeFile << std::endl;
            CloseHandle(hSnapshot);
            return true;
        }
        bRet = Process32Next(hSnapshot, &pe32);
    }

    CloseHandle(hSnapshot);
    return false;
}

void ThreadProc()
{
    while (true)
    {
        if (CheckProcess("x64dbg"))
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