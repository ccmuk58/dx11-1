////////////////////////////////////////////////////////////////////////////////
// 파일명: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
    m_Direct3D = 0;
    m_Camera = 0;
    m_Model = 0;
    m_ColorShader = 0;
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

    // 카메라 객체를 생성하고 초기 위치를 설정합니다.
    m_Camera = new CameraClass;
    m_Camera->SetPosition(0.0f, 0.0f, -5.0f); // 원점에서 Z축으로 5유닛 뒤

    // 모델 객체를 생성하고 초기화합니다.
    m_Model = new ModelClass;
    result = m_Model->Initialize(m_Direct3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // 컬러 셰이더 객체를 생성하고 초기화합니다.
    m_ColorShader = new ColorShaderClass;
    result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}
void ApplicationClass::Shutdown()
{
    if (m_ColorShader)
    {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = 0;
    }

    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

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
    if (!result) { return false; }

    return true;
}
bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    // 씬을 검은색으로 초기화합니다.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // 카메라 위치를 기반으로 뷰 행렬을 생성합니다.
    m_Camera->Render();

    // 월드, 뷰, 투영 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // 모델의 버텍스/인덱스 버퍼를 그래픽 파이프라인에 올립니다.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    // 컬러 셰이더로 모델을 렌더링합니다.
    result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(),
        m_Model->GetIndexCount(),
        worldMatrix, viewMatrix, projectionMatrix);
    if (!result) { return false; }

    // 렌더링된 씬을 화면에 표시합니다.
    m_Direct3D->EndScene();

    return true;
}