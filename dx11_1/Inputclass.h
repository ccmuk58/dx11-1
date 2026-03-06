#pragma once
////////////////////////////////////////////////////////////////////////////////
// 파일명: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    void Initialize();              // 모든 키 상태를 false(해제)로 초기화
    void KeyDown(unsigned int);     // 해당 키 상태를 true로
    void KeyUp(unsigned int);       // 해당 키 상태를 false로
    bool IsKeyDown(unsigned int);   // 특정 키가 현재 눌려있는지 반환

private:
    bool m_keys[256]; // 키보드 256개 키 상태 (true=눌림, false=해제)
};

#endif