////////////////////////////////////////////////////////////////////////////////
// 파일명: texture.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// 전역 변수 //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//////////////
// 타입 정의 //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex      : TEXCOORD0; // 색상 대신 텍스처 좌표 사용
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// 버텍스 셰이더
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

    // 행렬 계산을 위해 W를 1.0으로 설정합니다.
    input.position.w = 1.0f;

    // 버텍스 위치를 월드, 뷰, 투영 행렬로 변환합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 텍스처 좌표를 픽셀 셰이더로 전달합니다. (색상 대신)
    output.tex = input.tex;

    return output;
}