////////////////////////////////////////////////////////////////////////////////
// 파일명: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ModelClass.h"

ModelClass::ModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass& other) {}
ModelClass::~ModelClass() {}

bool ModelClass::Initialize(ID3D11Device* device)
{
    bool result;

    result = InitializeBuffers(device);
    if (!result) { return false; }

    return true;
}

void ModelClass::Shutdown()
{
    ShutdownBuffers();
    return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);
    return;
}

int ModelClass::GetIndexCount()
{
    return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // 버텍스와 인덱스 수를 설정합니다.
    m_vertexCount = 3;
    m_indexCount = 3;

    // 임시 버텍스 배열을 생성합니다.
    vertices = new VertexType[m_vertexCount];
    if (!vertices) { return false; }

    // 임시 인덱스 배열을 생성합니다.
    indices = new unsigned long[m_indexCount];
    if (!indices) { return false; }

    // 삼각형의 버텍스 데이터를 시계 방향으로 로드합니다.
    // 반시계 방향으로 입력하면 백페이스 컬링에 의해 보이지 않습니다.
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // 왼쪽 아래
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // 초록색

    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // 위쪽 가운데
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // 오른쪽 아래
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // 인덱스 데이터를 로드합니다.
    indices[0] = 0; // 왼쪽 아래
    indices[1] = 1; // 위쪽 가운데
    indices[2] = 2; // 오른쪽 아래

    // 버텍스 버퍼 설명을 설정합니다.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // 버텍스 데이터의 서브리소스 포인터를 설정합니다.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 버텍스 버퍼를 생성합니다.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result)) { return false; }

    // 인덱스 버퍼 설명을 설정합니다.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 인덱스 데이터의 서브리소스 포인터를 설정합니다.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼를 생성합니다.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result)) { return false; }

    // 버퍼에 데이터가 복사됐으므로 임시 배열을 해제합니다.
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

void ModelClass::ShutdownBuffers()
{
    if (m_indexBuffer) { m_indexBuffer->Release();  m_indexBuffer = 0; }
    if (m_vertexBuffer) { m_vertexBuffer->Release(); m_vertexBuffer = 0; }
    return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VertexType);
    offset = 0;

    // 버텍스 버퍼를 입력 어셈블러에 활성화합니다.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // 인덱스 버퍼를 입력 어셈블러에 활성화합니다.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 삼각형 리스트로 그리도록 설정합니다.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}