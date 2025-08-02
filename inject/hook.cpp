
#include <windows.h>
#include <winternl.h>
#include <iostream>
#include "MinHook.h"

typedef NTSTATUS(WINAPI* pNtSetInformationThread)(
    HANDLE          ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID           ThreadInformation,
    ULONG           ThreadInformationLength
);

pNtSetInformationThread OriginalNtSetInformationThread = nullptr;

NTSTATUS NTAPI HookedNtSetInformationThread(
    HANDLE ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength
) {
    // 拦截 ThreadHideFromDebugger（0x11）
    if (ThreadInformationClass == (THREADINFOCLASS)0x11) {
        std::cout << "Hook is running..." << std::endl;
        return 0; // 伪装调用成功
    }

    return OriginalNtSetInformationThread(
        ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength
    );
}

void Hook() {
    MH_Initialize();

    std::cout << "start to hook..." << std::endl;
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll) {
        void* pTarget = GetProcAddress(hNtdll, "NtSetInformationThread");
        if (pTarget) {
            MH_CreateHook(pTarget, &HookedNtSetInformationThread, (LPVOID*)&OriginalNtSetInformationThread);
            MH_EnableHook(pTarget);
        }
    }
}