//
//  TooltipScreen.h
//  MarbleGame
//
//  Created by Tarik Karaca on 30.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__TooltipScreen__
#define __MarbleGame__TooltipScreen__

#include "ScreenManager.h"
#include "UIScreen.h"
#include <string>
#include "VectorMath.h"

class TooltipScreen : public UIScreen {
    std::wstring m_text;
    Vector2 m_size;
    float m_time;
    Vector2 m_origin;
public:
    TooltipScreen(const std::wstring& text, Vector2 origin) : UIScreen(), m_text(text), m_origin(origin) { }
    
    void Render();
    void Initialize();
};

#endif /* defined(__MarbleGame__TooltipScreen__) */
