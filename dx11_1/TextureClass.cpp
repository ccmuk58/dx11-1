////////////////////////////////////////////////////////////////////////////////
// 파일명: TextureClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureClass.h"

TextureClass::TextureClass()
{
    m_targaData = 0;
    m_texture = 0;
    m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass& other) {}
TextureClass::~TextureClass() {}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
    char* filename)
{
    bool result;
    int height, width;
    D3D11_TEXTURE2D_DESC          textureDesc;
    HRESULT                       hResult;
    unsigned int                  rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    // Targa 파일을 m_targaData 배열로 로드합니다.
    result = LoadTarga32Bit(filename);
    if (!result) { return false; }

    // DirectX 텍스처 설명 구조체를 설정합니다.
    textureDesc.Height = m_height;
    textureDesc.Width = m_width;
    textureDesc.MipLevels = 0;          // 0 = 전체 밉맵 레벨 자동 생성
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT; // UpdateSubresource용
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 생성 허용

    // 빈 텍스처를 생성합니다.
    hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
    if (FAILED(hResult)) { return false; }

    // 행 피치를 계산합니다. (가로 픽셀 수 × 4바이트 RGBA)
    rowPitch = (m_width * 4) * sizeof(unsigned char);

    // Targa 데이터를 DirectX 텍스처에 복사합니다.
    // UpdateSubresource: 자주 변경되지 않는 데이터에 적합 (캐시 우선 메모리 사용)
    deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

    // 셰이더 리소스 뷰 설명을 설정합니다.
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1; // 전체 밉맵 레벨 사용

    // 셰이더 리소스 뷰를 생성합니다.
    hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
    if (FAILED(hResult)) { return false; }

    // 밉맵을 자동으로 생성합니다.
    deviceContext->GenerateMips(m_textureView);

    // 텍스처에 복사 완료됐으므로 원시 Targa 데이터를 해제합니다.
    delete[] m_targaData;
    m_targaData = 0;

    return true;
}

void TextureClass::Shutdown()
{
    if (m_textureView) { m_textureView->Release(); m_textureView = 0; }
    if (m_texture) { m_texture->Release();     m_texture = 0; }
    if (m_targaData) { delete[] m_targaData;    m_targaData = 0; }
    return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return m_textureView;
}
bool TextureClass::LoadTarga32Bit(char* filename)
{
    int error, bpp, imageSize, index, i, j, k;
    FILE* filePtr;
    unsigned int count;
    TargaHeader targaFileHeader;
    unsigned char* targaImage;

    // Targa 파일을 바이너리 모드로 엽니다.
    error = fopen_s(&filePtr, filename, "rb");
    if (error != 0) { return false; }

    // 파일 헤더를 읽습니다.
    count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    if (count != 1) { return false; }

    m_height = (int)targaFileHeader.height;
    m_width = (int)targaFileHeader.width;
    bpp = (int)targaFileHeader.bpp;

    // 32비트가 아니면 거부합니다. (알파 채널 필수)
    if (bpp != 32) { return false; }

    imageSize = m_width * m_height * 4;

    // 이미지 데이터를 읽을 임시 배열을 할당합니다.
    targaImage = new unsigned char[imageSize];

    count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
    if (count != imageSize) { return false; }

    error = fclose(filePtr);
    if (error != 0) { return false; }

    // 최종 저장용 배열을 할당합니다.
    m_targaData = new unsigned char[imageSize];

    index = 0;

    // Targa는 상하 반전 저장이므로 맨 아래 행부터 시작합니다.
    k = (m_width * m_height * 4) - (m_width * 4);

    for (j = 0; j < m_height; j++)
    {
        for (i = 0; i < m_width; i++)
        {
            // Targa는 BGRA 순서이므로 RGBA로 변환합니다.
            m_targaData[index + 0] = targaImage[k + 2]; // Red
            m_targaData[index + 1] = targaImage[k + 1]; // Green
            m_targaData[index + 2] = targaImage[k + 0]; // Blue
            m_targaData[index + 3] = targaImage[k + 3]; // Alpha

            k += 4;
            index += 4;
        }

        // 다음 행은 한 행 위로 (상하 반전이므로 역방향 이동)
        k -= (m_width * 8);
    }

    delete[] targaImage;
    targaImage = 0;

    return true;
}

int TextureClass::GetWidth() { return m_width; }
int TextureClass::GetHeight() { return m_height; }