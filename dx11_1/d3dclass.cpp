////////////////////////////////////////////////////////////////////////////////
// 파일명: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

D3DClass::D3DClass()
{
    m_swapChain = 0;
    m_device = 0;
    m_deviceContext = 0;
    m_renderTargetView = 0;
    m_depthStencilBuffer = 0;
    m_depthStencilState = 0;
    m_depthStencilView = 0;
    m_rasterState = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int     numModes, i, numerator, denominator;
    unsigned long long stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL    featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC          depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC      depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC         rasterDesc;
    float fieldOfView, screenAspect;

    m_vsync_enabled = vsync;

    // DirectX 그래픽 인터페이스 팩토리를 생성합니다.
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) { return false; }

    // 팩토리로 기본 그래픽 카드(어댑터)를 가져옵니다.
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) { return false; }

    // 기본 어댑터의 출력(모니터)을 가져옵니다.
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) { return false; }

    // DXGI_FORMAT_R8G8B8A8_UNORM 포맷에 맞는 디스플레이 모드 수를 가져옵니다.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) { return false; }

    // 가능한 모든 디스플레이 모드 목록을 저장할 배열을 만듭니다.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) { return false; }

    // 디스플레이 모드 목록을 채웁니다.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) { return false; }

    // 화면 크기와 일치하는 모드를 찾아 주사율의 분자/분모를 저장합니다.
    for (i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }
    // ========================== 2. GPU 정보 저장 및 리소스 해제 ===============================
    
    // 어댑터(GPU) 정보를 가져옵니다.
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) { return false; }

    // 전용 비디오 메모리를 MB 단위로 저장합니다.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // GPU 이름을 char 배열로 변환하여 저장합니다.
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128,
        adapterDesc.Description, 128);
    if (error != 0) { return false; }

    // 더 이상 필요 없는 리소스들을 해제합니다.
    delete[] displayModeList;
    displayModeList = 0;

    adapterOutput->Release();
    adapterOutput = 0;

    adapter->Release();
    adapter = 0;

    factory->Release();
    factory = 0;

    // ========================== 3. 스왑 체인 설정 ===============================

    // 스왑 체인 설명 구조체를 초기화합니다.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.BufferCount = 1;                          // 백버퍼 1개
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32비트 컬러

    // VSync 설정에 따라 주사율을 설정합니다.
    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;

    // 멀티샘플링 비활성화
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.Windowed = fullscreen ? false : true;

    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    // 피처 레벨을 DirectX 11로 설정합니다.
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // ========================== 4. 디바이스와 스왑 체인 생성 ===============================
    
    // 스왑 체인, Direct3D 디바이스, 디바이스 컨텍스트를 한 번에 생성합니다.
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
        &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
        &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result)) { return false; }

    // 백버퍼 포인터를 가져옵니다.
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result)) { return false; }

    // 백버퍼로 렌더 타겟 뷰를 생성합니다.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result)) { return false; }

    // 더 이상 필요 없으므로 백버퍼 포인터를 해제합니다.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // ========================== 5. 깊이/스텐실 버퍼 설정 ===============================

    // 깊이 버퍼 설명 구조체를 초기화합니다.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // 깊이 버퍼 텍스처를 생성합니다.
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result)) { return false; }

    // 깊이 스텐실 상태 설명 구조체를 초기화합니다.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // 앞면 픽셀에 대한 스텐실 연산
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // 뒷면 픽셀에 대한 스텐실 연산
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // 깊이 스텐실 상태를 생성합니다.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result)) { return false; }

    // 깊이 스텐실 상태를 적용합니다.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // 깊이 스텐실 뷰 설명 구조체를 초기화합니다.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // 깊이 스텐실 뷰를 생성합니다.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer,
        &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result)) { return false; }

    // 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩합니다.
    // 이제 파이프라인이 렌더링한 결과가 백버퍼에 기록됩니다.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // ========================== 6. 래스터라이저, 뷰포트, 행렬 설정 ===============================

    // 래스터라이저 상태를 설정합니다.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;  // 뒷면 컬링
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID; // 솔리드 채우기
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result)) { return false; }

    m_deviceContext->RSSetState(m_rasterState);

    // 뷰포트를 윈도우 전체 크기로 설정합니다.
    m_viewport.Width = (float)screenWidth;
    m_viewport.Height = (float)screenHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

    m_deviceContext->RSSetViewports(1, &m_viewport);

    // 투영 행렬을 생성합니다. (3D → 2D 뷰포트 변환)
    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect,
        screenNear, screenDepth);

    // 월드 행렬을 단위 행렬로 초기화합니다.
    m_worldMatrix = XMMatrixIdentity();

    // 직교 투영 행렬을 생성합니다. (2D UI 렌더링용)
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight,
        screenNear, screenDepth);

    return true;
}

void D3DClass::Shutdown()
{
    // 전체화면 상태에서 스왑 체인을 해제하면 예외가 발생하므로
    // 먼저 창 모드로 전환합니다.
    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if (m_rasterState) { m_rasterState->Release();       m_rasterState = 0; }
    if (m_depthStencilView) { m_depthStencilView->Release();  m_depthStencilView = 0; }
    if (m_depthStencilState) { m_depthStencilState->Release(); m_depthStencilState = 0; }
    if (m_depthStencilBuffer) { m_depthStencilBuffer->Release(); m_depthStencilBuffer = 0; }
    if (m_renderTargetView) { m_renderTargetView->Release();  m_renderTargetView = 0; }
    if (m_deviceContext) { m_deviceContext->Release();     m_deviceContext = 0; }
    if (m_device) { m_device->Release();            m_device = 0; }
    if (m_swapChain) { m_swapChain->Release();         m_swapChain = 0; }

    return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // 백버퍼를 지정한 색으로 지웁니다.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // 깊이 버퍼를 초기화합니다.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

void D3DClass::EndScene()
{
    if (m_vsync_enabled)
    {
        m_swapChain->Present(1, 0); // 주사율에 맞춰 표시
    }
    else
    {
        m_swapChain->Present(0, 0); // 가능한 빠르게 표시
    }

    return;
}

ID3D11Device* D3DClass::GetDevice()
{
    return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
    return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
    return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
    return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
    return;
}

void D3DClass::SetBackBufferRenderTarget()
{
    // 렌더 타겟 뷰와 깊이 스텐실 버퍼를 파이프라인에 바인딩합니다.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    return;
}

void D3DClass::ResetViewport()
{
    m_deviceContext->RSSetViewports(1, &m_viewport);
    return;
}