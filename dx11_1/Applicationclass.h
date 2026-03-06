#pragma once
////////////////////////////////////////////////////////////////////////////////
// 파일명: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "d3dclass.h"

// 그래픽 전역 설정
const bool  FULL_SCREEN = false;   // true: 전체화면 / false: 800x600 창 모드
const bool  VSYNC_ENABLED = true;    // 수직동기화 사용 여부
const float SCREEN_DEPTH = 1000.0f; // 원거리 클리핑 평면 (최대 렌더 거리)
const float SCREEN_NEAR = 0.3f;    // 근거리 클리핑 평면 (최소 렌더 거리)

class ApplicationClass
{
public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();

    bool Initialize(int, int, HWND); // 화면 너비, 높이, 윈도우 핸들로 초기화
    void Shutdown();
    bool Frame();

private:
    bool Render();

    // Tutorial 3부터 Direct3D 객체들이 여기에 추가됩니다.
private:
    D3DClass* m_Direct3D;
};

#endif