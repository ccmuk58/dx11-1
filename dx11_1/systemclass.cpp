////////////////////////////////////////////////////////////////////////////////
// 파일명: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

// 생성자: 포인터를 모두 null로 초기화합니다.
SystemClass::SystemClass()
{
    m_Input = 0;
    m_Application = 0;
}

// 복사 생성자: 컴파일러 자동 생성을 막기 위해 명시합니다.
SystemClass::SystemClass(const SystemClass& other)
{
}

// 소멸자: 정리는 Shutdown()에서 수행합니다.
SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
    int screenWidth, screenHeight;
    bool result;

    // InitializeWindows 내부에서 실제 화면 크기가 설정됩니다.
    screenWidth = 0;
    screenHeight = 0;

    // 윈도우를 생성합니다.
    InitializeWindows(screenWidth, screenHeight);

    // Input 객체 생성 및 초기화 (키보드 256키 상태 관리)
    m_Input = new InputClass;
    m_Input->Initialize();

    // Application 객체 생성 및 초기화 (DirectX 그래픽 담당)
    m_Application = new ApplicationClass;
    result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
    if (!result)
    {
        return false;
    }

    return true;
}

void SystemClass::Shutdown()
{
    if (m_Application)
    {
        m_Application->Shutdown();
        delete m_Application;
        m_Application = 0;
    }

    if (m_Input)
    {
        delete m_Input;
        m_Input = 0;
    }

    ShutdownWindows();

    return;
}

void SystemClass::Run()
{
    MSG  msg;
    bool done, result;

    ZeroMemory(&msg, sizeof(MSG));

    done = false;
    while (!done)
    {
        // 윈도우 메시지를 처리합니다.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // WM_QUIT 메시지가 오면 루프를 종료합니다.
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            // 매 프레임 처리를 수행합니다.
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }

    return;
}

bool SystemClass::Frame()
{
    bool result;

    // ESC 키를 누르면 애플리케이션을 종료합니다.
    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // Application 객체의 프레임 처리 (그래픽 렌더링 등)
    result = m_Application->Frame();
    if (!result)
    {
        return false;
    }

    return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        // 키보드 키가 눌렸을 때
    case WM_KEYDOWN:
    {
        m_Input->KeyDown((unsigned int)wparam);
        return 0;
    }

    // 키보드 키가 해제됐을 때
    case WM_KEYUP:
    {
        m_Input->KeyUp((unsigned int)wparam);
        return 0;
    }

    // 그 외는 윈도우 기본 핸들러로 처리합니다.
    default:
    {
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
    }
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE    dmScreenSettings;
    int        posX, posY;

    // WndProc에서 MessageHandler를 호출하기 위한 전역 포인터 설정
    ApplicationHandle = this;

    m_hinstance = GetModuleHandle(NULL);
    m_applicationName = L"Engine";

    // 윈도우 클래스를 기본 설정으로 구성합니다.
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // 검은 배경
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (FULL_SCREEN)
    {
        // 전체화면: 데스크톱 전체 크기, 32비트 색상
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        posX = posY = 0;
    }
    else
    {
        // 창 모드: 800x600, 화면 중앙 배치
        screenWidth = 800;
        screenHeight = 600;
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // 테두리 없는 팝업 스타일 윈도우를 생성합니다.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight,
        NULL, NULL, m_hinstance, NULL);

    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    ShowCursor(false); // 마우스 커서를 숨깁니다.

    return;
}
void SystemClass::ShutdownWindows()
{
    ShowCursor(true);

    // 전체화면이었다면 디스플레이 설정을 복원합니다.
    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    ApplicationHandle = NULL;

    return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    default:
    {
        return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
    }
    }
}