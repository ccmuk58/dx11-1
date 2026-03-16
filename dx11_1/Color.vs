////////////////////////////////////////////////////////////////////////////////
// 파일명: color.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// 전역 변수 //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;      // 월드 행렬
    matrix viewMatrix;       // 뷰 행렬
    matrix projectionMatrix; // 투영 행렬
};
//////////////
// 타입 정의 //
//////////////
struct VertexInputType
{
    float4 position : POSITION;  // 버텍스 셰이더 입력용 위치
    float4 color    : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION; // 픽셀 셰이더 입력용 위치
    float4 color    : COLOR;
};
////////////////////////////////////////////////////////////////////////////////
// 버텍스 셰이더
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;

    // 행렬 계산을 위해 위치 벡터의 W를 1.0으로 설정합니다.
    input.position.w = 1.0f;

    // 버텍스 위치를 월드, 뷰, 투영 행렬로 변환합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 색상을 픽셀 셰이더로 전달합니다.
    output.color = input.color;

    return output;
}