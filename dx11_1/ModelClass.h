////////////////////////////////////////////////////////////////////////////////
// 파일명: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "TextureClass.h" // 새로 추가

using namespace DirectX;

class ModelClass
{
private:
    // 색상(XMFLOAT4) → 텍스처 좌표(XMFLOAT2)로 변경
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture; // UV 텍스처 좌표
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    // 텍스처 파일명과 디바이스 컨텍스트가 추가되었습니다.
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown();
    void Render(ID3D11DeviceContext*); // 버퍼를 그래픽 파이프라인에 올림

    int GetIndexCount(); // 인덱스 수 반환 (셰이더가 사용)
    ID3D11ShaderResourceView* GetTexture(); // 텍스처 리소스 반환 (새로 추가)

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*); // 새로 추가
    void ReleaseTexture();                                         // 새로 추가

private:
    ID3D11Buffer* m_vertexBuffer; // 버텍스 버퍼
    ID3D11Buffer* m_indexBuffer;  // 인덱스 버퍼
    int m_vertexCount;
    int m_indexCount;
    TextureClass* m_Texture; // 텍스처 객체 (새로 추가)
};

#endif