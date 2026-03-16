#pragma once
////////////////////////////////////////////////////////////////////////////////
// 파일명: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <directxmath.h>
using namespace DirectX;

class CameraClass
{
public:
    CameraClass();
    CameraClass(const CameraClass&);
    ~CameraClass();

    void SetPosition(float, float, float); // 카메라 위치 설정
    void SetRotation(float, float, float); // 카메라 회전 설정

    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();

    void Render();                        // 뷰 행렬 생성
    void GetViewMatrix(XMMATRIX&);        // 뷰 행렬 반환

private:
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;
    XMMATRIX m_viewMatrix;
};

#endif