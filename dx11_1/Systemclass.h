#pragma once
////////////////////////////////////////////////////////////////////////////////
// 파일명: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// 잘 쓰이지 않는 Win32 API를 제외해 빌드 속도를 높입니다.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "inputclass.h"
#include "applicationclass.h"

class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR    m_applicationName;
    HINSTANCE  m_hinstance;
    HWND       m_hwnd;

    InputClass* m_Input;       // 키보드 입력 객체
    ApplicationClass* m_Application; // 그래픽 애플리케이션 객체
};

// WndProc: 윈도우 메시지를 MessageHandler로 전달하는 전역 콜백 함수
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ApplicationHandle: WndProc에서 SystemClass에 접근하기 위한 전역 포인터
static SystemClass* ApplicationHandle = 0;

#endif