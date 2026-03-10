////////////////////////////////////////////////////////////////////////////////
// 파일명: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/////////////
// 라이브러리 링크 //
/////////////
#pragma comment(lib, "d3d11.lib")       // DirectX 11 핵심 기능
#pragma comment(lib, "dxgi.lib")        // 하드웨어 정보 (모니터 주사율, GPU 등)
#pragma comment(lib, "d3dcompiler.lib") // 셰이더 컴파일

//////////////
// 헤더 포함 //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class D3DClass
{
public:
    D3DClass();
    D3DClass(const D3DClass&);
    ~D3DClass();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float); // 프레임 시작 시 버퍼 초기화
    void EndScene();                             // 프레임 끝에 백버퍼를 화면에 표시

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX&); // 투영 행렬 반환
    void GetWorldMatrix(XMMATRIX&);      // 월드 행렬 반환
    void GetOrthoMatrix(XMMATRIX&);      // 직교 투영 행렬 반환

    void GetVideoCardInfo(char*, int&);  // GPU 이름과 메모리 반환

    void SetBackBufferRenderTarget();    // 렌더 타겟을 백버퍼로 설정
    void ResetViewport();               // 뷰포트 초기화

private:
    bool  m_vsync_enabled;
    int   m_videoCardMemory;
    char  m_videoCardDescription[128];

    IDXGISwapChain* m_swapChain;                 // 스왑 체인
    ID3D11Device* m_device;                      // D3D 디바이스
    ID3D11DeviceContext* m_deviceContext;        // D3D 디바이스 컨텍스트
    ID3D11RenderTargetView* m_renderTargetView;  // 렌더 타겟 뷰
    ID3D11Texture2D* m_depthStencilBuffer;       // 깊이/스텐실 버퍼
    ID3D11DepthStencilState* m_depthStencilState; // 깊이/스텐실 상태
    ID3D11DepthStencilView* m_depthStencilView;  // 깊이/스텐실 뷰
    ID3D11RasterizerState* m_rasterState;       // 래스터라이저 상태

    XMMATRIX m_projectionMatrix; // 투영 행렬
    XMMATRIX m_worldMatrix;      // 월드 행렬
    XMMATRIX m_orthoMatrix;      // 직교 투영 행렬 (2D UI용)

    D3D11_VIEWPORT m_viewport;   // 뷰포트
};

#endif