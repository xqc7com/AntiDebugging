#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>

BOOL InjectDLL(DWORD dwPID, const char* dllPath) 
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    if (!hProcess) 
    {
        std::cerr << "OpenProcess failed\n";
        return FALSE;
    }

    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!pRemoteMemory)
    {
        std::cerr << "VirtualAllocEx failed\n";
        CloseHandle(hProcess);
        return FALSE;
    }

    WriteProcessMemory(hProcess, pRemoteMemory, dllPath, strlen(dllPath) + 1, nullptr);

    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, pLoadLibrary, pRemoteMemory, 0, nullptr);
    if (!hThread) 
    {
        std::cerr << "CreateRemoteThread failed\n";
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}

int main(int argc, char** argv)
{
    if (argc != 3) 
    {
        std::cout << "Usage: <MyHook.exe> <target.exe> <inject.dll>";
        return -1;
    }

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcess(argv[1], NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

    const char* dllPath = argv[2];
    if (InjectDLL(pi.dwProcessId, dllPath))
    {
        std::cout << "DLL injected successfully!\n";
    }
    else
    {
        std::cerr << "DLL injection failed.\n";
    }

    ResumeThread(pi.hThread);
    std::cout << "process " << argv[1] << " is running..." << std::endl;
    return 0;
}