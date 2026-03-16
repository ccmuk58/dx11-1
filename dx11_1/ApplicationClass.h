#pragma once
////////////////////////////////////////////////////////////////////////////////
// 파일명: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

///////////////////////
// 클래스 헤더 포함   //
///////////////////////
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"

/////////////
// 전역 상수 //
/////////////
const bool  FULL_SCREEN = false;
const bool  VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();

private:
    bool Render();

private:
    D3DClass* m_Direct3D;  // D3DClass 포인터
    CameraClass* m_Camera;      // 카메라 (새로 추가)
    ModelClass* m_Model;       // 3D 모델 (새로 추가)
    TextureShaderClass* m_TextureShader; // ColorShaderClass -> TextureShaderClass
};

#endif