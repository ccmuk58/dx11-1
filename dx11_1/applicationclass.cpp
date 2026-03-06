////////////////////////////////////////////////////////////////////////////////
// 파일명: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"

ApplicationClass::ApplicationClass() 
{
    m_Direct3D = 0; // 안전을 위해 null로 초기화
}
ApplicationClass::ApplicationClass(const ApplicationClass& other) 
{
    
}
ApplicationClass::~ApplicationClass() 
{

}

// 아래 함수들은 현재 비어 있습니다. Tutorial 3부터 내용이 채워집니다.
bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;
    // Direct3D 객체 생성후 초기화
    m_Direct3D = new D3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    return true;
}

void ApplicationClass::Shutdown()
{
    // dircet3D 객체 해제
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

    result = Render();
    if (!result)
    {
        return false;
    }
    return true;
}

bool ApplicationClass::Render()
{
    // 씬 시작 전 버퍼를 회색으로 초기화
    m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

    // 렌더링된 씬을 화면에 표시
    m_Direct3D->EndScene();

    return true;
}