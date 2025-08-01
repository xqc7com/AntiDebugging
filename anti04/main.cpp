#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include <winternl.h>

bool CheckProcessIsDebuging()
{
    //return IsDebuggerPresent();

    BOOL isDebugging = FALSE;
    HANDLE hHandle = GetCurrentProcess();
    CheckRemoteDebuggerPresent(hHandle, &isDebugging);
    return isDebugging;
}

void ThreadProc()
{
    while (true)
    {
        if (CheckProcessIsDebuging())
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