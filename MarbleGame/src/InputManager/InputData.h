//
//  InputData.h
//  MarbleGame
//
//  Created by Tarik Karaca on 03.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__InputData__
#define __MarbleGame__InputData__

#include <memory>

enum InputTypes {
    InputTypesKey,
    InputTypesMButton,
    InputTypesMMotion,
    InputTypesMScroll,
    InputTypesLength
};

#define MODIFIER_SHIFT 0x1
#define MODIFIER_CTRL 0x2
#define MODIFIER_ALT 0x4
#define MODIFIER_CMD 0x8

enum MButton {
    MButtonLeft,
    MButtonRight
};

typedef unsigned char Modifier;

class IInputData {
protected:
    IInputData(int inputType) : m_inputType(inputType) { }
public:
    const int m_inputType;
};

class KeyInputData : public IInputData {
public:
    const wchar_t m_keyCode;
    const Modifier m_modifierFlags;
    const bool m_state;
    
    KeyInputData(unsigned char key, Modifier mod, bool state)
            : IInputData(InputTypesKey), m_keyCode(key),
                m_modifierFlags(mod), m_state(state) { }
};

class MButtonInputData : public IInputData {
public:
    const MButton m_button;
    const Modifier m_modifierFlags;
    const bool m_state;
    
    MButtonInputData(MButton button, Modifier mod, bool state)
            : IInputData(InputTypesMButton), m_button(button),
                m_modifierFlags(mod), m_state(state) { }
};

class MMotionInputData : public IInputData {
public:
    const int m_x, m_y;
    const int m_deltaX, m_deltaY;
    
    MMotionInputData(int x, int y, int deltaX, int deltaY)
            : IInputData(InputTypesMMotion), m_x(x),
                m_y(y), m_deltaX(deltaX), m_deltaY(deltaY) { }
};

class MScrollInputData : public IInputData {
public:
    const float m_deltaX, m_deltaY;
    
    MScrollInputData(float deltaX, float deltaY)
            : IInputData(InputTypesMScroll), m_deltaX(deltaX),
                m_deltaY(deltaY) { }
};

typedef void (*KeyInputHandle)(const KeyInputData&);
typedef void (*MButtonInputHandle)(const MButtonInputData&);
typedef void (*MMotionInputHandle)(const MMotionInputData&);
typedef void (*MScrollInputHandle)(const MScrollInputData&);

typedef std::shared_ptr<IInputData> IInputDataPtr;

#endif /* defined(__MarbleGame__InputData__) */
