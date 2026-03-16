////////////////////////////////////////////////////////////////////////////////
// 파일명: TextureClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <stdio.h>

class TextureClass
{
private:
    // Targa 파일 헤더 구조체 (파일 읽기를 쉽게 하기 위해 정의)
    struct TargaHeader
    {
        unsigned char  data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char  bpp;   // bits per pixel (32비트만 지원)
        unsigned char  data2;
    };

public:
    TextureClass();
    TextureClass(const TextureClass&);
    ~TextureClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture(); // 셰이더가 사용할 텍스처 뷰 반환
    int GetWidth();
    int GetHeight();

private:
    bool LoadTarga32Bit(char*); // Targa 파일 로드 함수 (32비트만 지원)

private:
    unsigned char* m_targaData;   // 원시 Targa 데이터
    ID3D11Texture2D* m_texture;     // DirectX 텍스처
    ID3D11ShaderResourceView* m_textureView; // 셰이더 리소스 뷰
    int m_width, m_height;
};

#endif