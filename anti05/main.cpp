#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>

bool CheckProcessIsDebugging()
{
    BOOL isx64 = FALSE;
    HKEY hKey = NULL;
    char key[] = "Debugger";
    char regValue[MAX_PATH] = { 0 };
    DWORD dwType = 0, dwLegth = MAX_PATH;
    char reg32[] = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";
    char reg64[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";

    //判断当前进程版本
    IsWow64Process(GetCurrentProcess(), &isx64);

    RegCreateKey(HKEY_LOCAL_MACHINE, isx64 ? reg64 : reg32, &hKey);
    RegQueryValueEx(hKey, key, NULL, &dwType, (LPBYTE)regValue, &dwLegth);
    return (strstr(regValue, "debugger") != NULL);
}

void ThreadProc()
{
    while (true)
    {
        if (CheckProcessIsDebugging())
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