////////////////////////////////////////////////////////////////////////////////
// 파일명: TextureShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
    m_sampleState = 0; // 새로 추가
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other) {}
TextureShaderClass::~TextureShaderClass() {}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;

    // 버텍스 셰이더 파일명을 설정합니다.
    error = wcscpy_s(vsFilename, 128, L"../dx11_1/Texture.vs");
    if (error != 0) { return false; }

    // 픽셀 셰이더 파일명을 설정합니다.
    error = wcscpy_s(psFilename, 128, L"../dx11_1/Texture.ps");
    if (error != 0) { return false; }

    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if (!result) { return false; }

    return true;
}

void TextureShaderClass::Shutdown()
{
    ShutdownShader();
    return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
    XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    bool result;

    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix,
        projectionMatrix, texture);
    if (!result) { return false; }

    RenderShader(deviceContext, indexCount);

    return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
    WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = 0;
    ID3D10Blob* vertexShaderBuffer = 0;
    ID3D10Blob* pixelShaderBuffer = 0;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC    matrixBufferDesc;
    D3D11_SAMPLER_DESC   samplerDesc; // 새로 추가

    // 버텍스 셰이더를 컴파일합니다.
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
        else { MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); }
        return false;
    }

    // 픽셀 셰이더를 컴파일합니다.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
        else { MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK); }
        return false;
    }

    // 버텍스/픽셀 셰이더 객체를 생성합니다.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        NULL, &m_vertexShader);
    if (FAILED(result)) { return false; }

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(),
        NULL, &m_pixelShader);
    if (FAILED(result)) { return false; }

    // 입력 레이아웃을 설정합니다.
    // 두 번째 요소가 COLOR → TEXCOORD, float4 → float2로 변경되었습니다.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD"; // COLOR → TEXCOORD
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT; // float2 (UV)
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements,
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result)) { return false; }

    vertexShaderBuffer->Release(); vertexShaderBuffer = 0;
    pixelShaderBuffer->Release();  pixelShaderBuffer = 0;

    // 상수 버퍼를 설정합니다.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result)) { return false; }

    // 샘플러 상태를 설정합니다.
    // Filter: 축소/확대/밉맵 모두 선형 보간 (최고 화질, 처리 비용 높음)
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    // AddressU/V/W: Wrap → 0.0~1.0 범위를 벗어나면 다시 처음으로 되돌아옴
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result)) { return false; }

    return true;
}

void TextureShaderClass::ShutdownShader()
{
    if (m_sampleState) { m_sampleState->Release();  m_sampleState = 0; }
    if (m_matrixBuffer) { m_matrixBuffer->Release(); m_matrixBuffer = 0; }
    if (m_layout) { m_layout->Release();       m_layout = 0; }
    if (m_pixelShader) { m_pixelShader->Release();  m_pixelShader = 0; }
    if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = 0; }
    return;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage,
    HWND hwnd, WCHAR* shaderFilename)
{
    char* compileErrors = (char*)(errorMessage->GetBufferPointer());
    unsigned long long bufferSize = errorMessage->GetBufferSize();
    ofstream fout;

    fout.open("shader-error.txt");
    for (unsigned long long i = 0; i < bufferSize; i++) { fout << compileErrors[i]; }
    fout.close();

    errorMessage->Release();
    errorMessage = 0;

    MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.",
        shaderFilename, MB_OK);
    return;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
    XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    // DirectX 11에서는 셰이더 전달 전 행렬을 전치해야 합니다.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) { return false; }

    dataPtr = (MatrixBufferType*)mappedResource.pData;
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(m_matrixBuffer, 0);

    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // 픽셀 셰이더에 텍스처 리소스를 설정합니다. (새로 추가)
    deviceContext->PSSetShaderResources(0, 1, &texture);

    return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_layout);
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 픽셀 셰이더에 샘플러 상태를 설정합니다. (새로 추가)
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    deviceContext->DrawIndexed(indexCount, 0, 0);
    return;
}