////////////////////////////////////////////////////////////////////////////////
// 파일명: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class ModelClass
{
private:
    // ColorShaderClass의 입력 레이아웃과 반드시 일치해야 합니다.
    struct VertexType
    {
        XMFLOAT3 position; // 위치 (x, y, z)
        XMFLOAT4 color;    // 색상 (r, g, b, a)
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device*);   // 버텍스/인덱스 버퍼 초기화
    void Shutdown();
    void Render(ID3D11DeviceContext*); // 버퍼를 그래픽 파이프라인에 올림

    int GetIndexCount(); // 인덱스 수 반환 (셰이더가 사용)

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    ID3D11Buffer* m_vertexBuffer; // 버텍스 버퍼
    ID3D11Buffer* m_indexBuffer;  // 인덱스 버퍼
    int m_vertexCount;
    int m_indexCount;
};

#endif