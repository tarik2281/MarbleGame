//
//  ButtonUI.h
//  MarbleGame
//
//  Created by Tarik Karaca on 09.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__ButtonUI__
#define __MarbleGame__ButtonUI__

#include "ControlUI.h"
#include <string>

#include "Delegate.h"

#include "ScreenManager.h"

typedef Delegate<void> ButtonClickDelegate;

class Texture2D;

#define LEFT 11
#define RIGHT 11
#define TOP 11
#define BOTTOM 11
#define MARGIN 10

class ButtonStyle {
    Texture2D* m_background;
    Texture2D* m_hoverBackground;
    
    Vector3 m_textColor;
    Vector3 m_textHoverColor;
};


class ButtonUI : public ControlUI {
    ScreenPtr m_tooltip = nullptr;
    bool m_hover;
public:
    ButtonUI() { }
    ~ButtonUI() { if (m_tooltip && m_showTooltip) m_tooltip->ExitScreen(); }
    
    void Render(float opacity);
    
    bool HandleMButtonEvent(const MButtonInputData&);
    bool HandleMMotionEvent(const MMotionInputData&);
    
    ButtonClickDelegate m_delegate;
    std::wstring m_text;
    bool m_showTooltip = true;
    
    void SetText(const std::wstring&);
    
    Texture2D* m_background = 0;
    Texture2D* m_hoverBackground = 0;
};

typedef std::shared_ptr<ButtonUI> ButtonPtr;

#endif /* defined(__MarbleGame__ButtonUI__) */
