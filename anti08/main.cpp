
#include <iostream>
#include <thread>
#include <string>
#include <windows.h>  

bool CheckProcessIsDebugging()
{
    bool isDebugged = true;
    __try {
        _asm int 3;
        //int a = 1;
        //int b = 0;
        //volatile int c = a / b;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        isDebugged = false;
    }
    return isDebugged;
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