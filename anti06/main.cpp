
#include <iostream>
#include <thread>
#include <string>
#include <windows.h>
#include <winternl.h>

typedef NTSTATUS(WINAPI* FnNtQueryInformationProcess)(
    HANDLE           ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID            ProcessInformation,
    ULONG            ProcessInformationLength,
    PULONG           ReturnLength
    );

bool CheckNtQueryInformationProcess()
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    HMODULE hModule = LoadLibrary("ntdll.dll");
    if ((hProcess == INVALID_HANDLE_VALUE) || (hModule == INVALID_HANDLE_VALUE))
    {
        return false;
    }

    FnNtQueryInformationProcess fnProcess = (FnNtQueryInformationProcess)GetProcAddress(hModule, "NtQueryInformationProcess");
    if (fnProcess != INVALID_HANDLE_VALUE)
    {
        //0x00 检测PEB结构的调试标志
        /*
        PROCESS_BASIC_INFORMATION pbi = { 0 };
        DWORD dwSize = sizeof(PROCESS_BASIC_INFORMATION);
        DWORD dwRetLength = 0;
        fnProcess(hProcess, ProcessBasicInformation, &pbi, dwSize, &dwRetLength);
        PPEB peb = pbi.PebBaseAddress;
        if (peb != NULL)
        {
            return peb->BeingDebugged;
        }
        */

        //0x07 检测调试端口
        ULONG_PTR dwDebugPort = 0;
        fnProcess(hProcess, ProcessDebugPort, &dwDebugPort, sizeof(dwDebugPort), NULL);
        return (dwDebugPort != 0);

        //0x1E 检测调试句柄
        /*
        HANDLE hHandle = NULL;
        fnProcess(hProcess, (PROCESSINFOCLASS)0x1E, &hHandle, sizeof(hHandle), NULL);
        return (hHandle != NULL);
        */

        //0x1F 检测调试标志
        /*
        BOOL bDebugFlag = FALSE;
        fnProcess(hProcess, (PROCESSINFOCLASS)0x1F, &bDebugFlag, sizeof(bDebugFlag), NULL);
        return (bDebugFlag == FALSE);
        */
    }
    return false;
}

void ThreadProc()
{
    while (true)
    {
        if (CheckNtQueryInformationProcess())
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