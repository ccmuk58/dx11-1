#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class SystemClass
{
public:
    SystemClass();
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR     m_applicationName;
    HINSTANCE   m_hinstance;
    HWND        m_hwnd;        
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;