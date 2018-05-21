//
//  TextBoxUI.h
//  MarbleGame
//
//  Created by Tarik Karaca on 31.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__TextBoxUI__
#define __MarbleGame__TextBoxUI__

#include "ControlUI.h"

#include <string>

class TextBoxUI : public ControlUI {
    bool m_hover;
    bool m_focus;
    std::wstring m_text;
public:
    bool HandleKeyEvent(const KeyInputData&);
    bool HandleMButtonEvent(const MButtonInputData&);
    bool HandleMMotionEvent(const MMotionInputData&);
    
    void Render(float opacity);
};

#endif /* defined(__MarbleGame__TextBoxUI__) */
