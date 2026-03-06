////////////////////////////////////////////////////////////////////////////////
// 파일명: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass() {}
InputClass::InputClass(const InputClass& other) {}
InputClass::~InputClass() {}

void InputClass::Initialize()
{
    int i;

    // 모든 키를 해제 상태(false)로 초기화합니다.
    for (i = 0; i < 256; i++)
    {
        m_keys[i] = false;
    }

    return;
}

void InputClass::KeyDown(unsigned int input)
{
    // 키가 눌렸을 때 해당 키 상태를 true로 저장합니다.
    m_keys[input] = true;
    return;
}

void InputClass::KeyUp(unsigned int input)
{
    // 키가 해제됐을 때 해당 키 상태를 false로 저장합니다.
    m_keys[input] = false;
    return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
    // 특정 키가 현재 눌려있는지 반환합니다.
    return m_keys[key];
}