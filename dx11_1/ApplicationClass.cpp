////////////////////////////////////////////////////////////////////////////////
// 파일명: ApplicationClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
    m_Direct3D     = 0;
    m_Camera       = 0;
    m_Model        = 0;
    m_TextureShader = 0; // ColorShader → TextureShader
}

ApplicationClass::ApplicationClass(const ApplicationClass& other) {}
ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char textureFilename[128];
    bool result;

    m_Direct3D = new D3DClass;
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED,
                                    hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new CameraClass;
    m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

    m_Model = new ModelClass;

    // 텍스처 파일명을 설정하고 모델을 초기화합니다.
    strcpy_s(textureFilename, "../dx11_1/data/stone01.tga");

    result = m_Model->Initialize(m_Direct3D->GetDevice(),
                                  m_Direct3D->GetDeviceContext(), textureFilename);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // TextureShaderClass 객체를 생성하고 초기화합니다.
    m_TextureShader = new TextureShaderClass;

    result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}

void ApplicationClass::Shutdown()
{
    if(m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    if(m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    if(m_Camera) { delete m_Camera; m_Camera = 0; }

    if(m_Direct3D)
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
    if(!result) { return false; }
    return true;
}

bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Model->Render(m_Direct3D->GetDeviceContext());

    // 텍스처 셰이더로 모델을 렌더링합니다.
    // 모델의 텍스처 리소스 포인터도 함께 전달합니다.
    result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
                                     m_Model->GetIndexCount(),
                                     worldMatrix, viewMatrix, projectionMatrix,
                                     m_Model->GetTexture()); // 텍스처 추가
    if(!result) { return false; }

    m_Direct3D->EndScene();

    return true;
}