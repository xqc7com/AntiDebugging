
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>

BOOL isDebugging = FALSE;
std::string g_szWindowsTile = "";

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char windows_title[256] = { 0 };
    char class_name[256] = { 0 };
    GetWindowText(hwnd, windows_title, 256);
    if (strlen(windows_title) > 0 && strstr(windows_title, g_szWindowsTile.c_str()) != 0)
    {
        std::cout << "handle:" << hwnd << " find debug windows:" << windows_title << std::endl;
        isDebugging = TRUE;
    }
    return TRUE;
}

bool CheckDebugWindowByEnum(const char* szWindowTitle)
{
    isDebugging = false;
    g_szWindowsTile = szWindowTitle;
    EnumWindows(EnumWindowsProc, NULL);
    return isDebugging;
}

bool CheckDebugWindowByFind(const char* szWindowName)
{
    std::vector<HWND> vec;
    HWND hWnd = GetTopWindow(0);
    while (hWnd)
    {
        if (GetParent(hWnd) == 0)
        {
            vec.push_back(hWnd);
        }
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }

    char szTempName[MAX_PATH] = { 0 };
    for (auto& v : vec)
    {
        GetWindowTextA(v, szTempName, MAX_PATH);
        if (strstr(szTempName, szWindowName) != 0)
        {
            std::cout << "find debug windows:" << szTempName << std::endl;
            return true;
        }
        memset(szTempName, 0, sizeof(szTempName));
    }

    return false;
}

void ThreadProc()
{
    while (true)
    {
        if (CheckDebugWindowByEnum("x64dbg")) //CheckDebugWindowByFind("x64dbg")) //
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