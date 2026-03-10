#include "System.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR pScmdline, int iCmdshow)
{
    SystemClass* System = new SystemClass();

    if (System->Initialize())
    {
        System->Run();
    }

    System->Shutdown();
    delete System;
    System = nullptr;

    return 0;
}