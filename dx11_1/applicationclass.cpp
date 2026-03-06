////////////////////////////////////////////////////////////////////////////////
// 파일명: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"

ApplicationClass::ApplicationClass() 
{
    m_Direct3D = 0; // 안전을 위해 null로 초기화
    // 
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
    return true;
}

void ApplicationClass::Shutdown()
{
    return;
}

bool ApplicationClass::Frame()
{
    return true;
}

bool ApplicationClass::Render()
{
    return true;
}