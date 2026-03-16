////////////////////////////////////////////////////////////////////////////////
// 파일명: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CameraClass.h"

CameraClass::CameraClass()
{
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_positionZ = 0.0f;

    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other) {}
CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z)
{
    m_positionX = x;
    m_positionY = y;
    m_positionZ = z;
    return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
    return;
}

XMFLOAT3 CameraClass::GetPosition()
{
    return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
    return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
    XMFLOAT3 up, position, lookAt;
    XMVECTOR upVector, positionVector, lookAtVector;
    float yaw, pitch, roll;
    XMMATRIX rotationMatrix;

    // 위쪽 방향 벡터를 설정합니다. (Y축이 위)
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    upVector = XMLoadFloat3(&up);

    // 카메라 위치를 설정합니다.
    position.x = m_positionX;
    position.y = m_positionY;
    position.z = m_positionZ;
    positionVector = XMLoadFloat3(&position);

    // 기본 시선 방향을 설정합니다. (Z+ 방향)
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;
    lookAtVector = XMLoadFloat3(&lookAt);

    // 회전값을 도(degree)에서 라디안(radian)으로 변환합니다.
    pitch = m_rotationX * 0.0174532925f; // X축 회전 (상하)
    yaw = m_rotationY * 0.0174532925f; // Y축 회전 (좌우)
    roll = m_rotationZ * 0.0174532925f; // Z축 회전 (기울기)

    // yaw, pitch, roll 값으로 회전 행렬을 만듭니다.
    rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // lookAt과 up 벡터를 회전 행렬로 변환해 카메라 방향을 올바르게 설정합니다.
    lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
    upVector = XMVector3TransformCoord(upVector, rotationMatrix);

    // 회전된 카메라 시선을 카메라 위치에 맞게 이동합니다.
    lookAtVector = XMVectorAdd(positionVector, lookAtVector);

    // 뷰 행렬을 생성합니다.
    m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

    return;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
    return;
}