#include "System.h"

LRESULT CALLBACK SystemClass::MessageHandler(HWND, UINT, WPARAM, LPARAM) {}
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    ApplicationHandle = this;
    m_hinstance = GetModuleHandle(NULL);
    m_applicationName = L"DX11";

    // 윈도우 클래스 등록
    WNDCLASSEX wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;           // 메시지 처리 함수 등록
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);

    // 해상도 가져오기
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 윈도우 생성
    m_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,  // 풀스크린용
        0, 0, screenWidth, screenHeight,
        NULL, NULL, m_hinstance, NULL
    );

    // 윈도우 표시
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}
void SystemClass::Run()
{
    MSG msg = {};

    while (true)
    {
        // Windows 메시지 처리 (키입력, 마우스 등)
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) break;

        // 실제 게임/렌더링 로직
        if (!Frame()) break;
    }
}
// Win32가 이벤트 발생시 여기로 보냄
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    default:
        return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
    }
}