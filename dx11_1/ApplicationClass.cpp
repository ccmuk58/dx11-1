////////////////////////////////////////////////////////////////////////////////
// 파일명: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
    m_Direct3D = 0;  // 안전을 위해 null로 초기화
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}
bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;

    // Direct3D 객체를 생성하고 초기화합니다.
    m_Direct3D = new D3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED,
        hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    return true;
}
void ApplicationClass::Shutdown()
{
    // Direct3D 객체를 해제합니다.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }

    return;
}
bool ApplicationClass::Frame()
{
    bool result;

    // 그래픽 씬을 렌더링합니다.
    result = Render();
    if (!result)
    {
        return false;
    }

    return true;
}
bool ApplicationClass::Render()
{
    // 씬 시작 전 버퍼를 회색(R:0.5 G:0.5 B:0.5 A:1.0)으로 초기화합니다.
    m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

    // 렌더링된 씬을 화면에 표시합니다.
    m_Direct3D->EndScene();

    return true;
}