#include <iostream>
#include <thread>
#include <string>
#include <windows.h>
#include <TlHelp32.h>
#include <winternl.h>

typedef NTSTATUS(WINAPI* FnNtSetInformationThread)(
    HANDLE          ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID           ThreadInformation,
    ULONG           ThreadInformationLength
);

bool SetThreadInfo(HANDLE hThread)
{
    HMODULE hModule = LoadLibrary("ntdll.dll");
    FnNtSetInformationThread fn = (FnNtSetInformationThread)GetProcAddress(hModule, "NtSetInformationThread");
    NTSTATUS status = fn(hThread, (THREADINFOCLASS)0x11, 0, 0);
    return (status == 0);
}

bool SetInformationThread()
{
    DWORD dwProcessId = GetProcessId(GetCurrentProcess());
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return false;

    THREADENTRY32 th32;
    th32.dwSize = sizeof(THREADENTRY32);
    BOOL bRet = Thread32First(hThreadSnap, &th32);
    while (bRet)
    {
        if (th32.th32OwnerProcessID == dwProcessId)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
            bool ok = SetThreadInfo(hThread);
            if (ok) {
                std::cout << "hide thread succ:" << th32.th32ThreadID << std::endl;
            }
            else {
                std::cout << "hide thread fail:" << th32.th32ThreadID << std::endl;
            }
            CloseHandle(hThread);
        }
        bRet = Thread32Next(hThreadSnap, &th32);
    }
    CloseHandle(hThreadSnap);
    return false;
}

void AntiThreadProc()
{
    while (true)
    {
        SetInformationThread(); //设置反调试
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

void ThreadProc()
{
    while (true)
    {
        std::cout << "business Running, thread:" << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

int main()
{
    std::thread thrd1(AntiThreadProc); 
    std::thread thrd2(ThreadProc);
    thrd1.join();
    thrd2.join();
    return 0;
}