//
//  TextBoxUI.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 31.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "TextBoxUI.h"

#include "UIScreen.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Font.h"

#include "Util.h"

bool TextBoxUI::HandleKeyEvent(const KeyInputData &data) {
    if (m_focus && data.m_state) {
        if (data.m_keyCode == 0x7f) {
            if (m_text.length() > 0) m_text.pop_back();
            return true;
        }
        m_text += data.m_keyCode;
        return true;
    }
    return false;
}

bool TextBoxUI::HandleMButtonEvent(const MButtonInputData &data) {
    if (data.m_button == MButtonLeft && data.m_state) {
        if (m_hover) {
            m_focus = true;
            g_pInputManager->AddKeyInputHandler(HotKey::k_Wildcard, m_owner);
            return true;
        } else {
            m_focus = false;
            g_pInputManager->RemoveKeyInputHandler(HotKey::k_Wildcard);
            return false;
        }
    }
    
    return false;
}

bool TextBoxUI::HandleMMotionEvent(const MMotionInputData &data) {
    m_hover = (data.m_x > m_origin.x - m_halfSize.x && data.m_x < m_origin.x + m_halfSize.x &&
               data.m_y > m_origin.y - m_halfSize.y && data.m_y < m_origin.y + m_halfSize.y);
    return m_hover;
}

void TextBoxUI::Render(float opacity) {
    glBindTexture(GL_TEXTURE_2D, 0);
        
    glColor4f((m_focus) ? 1.0f : 0.0, 1.0f, 1.0f, opacity);
    glBegin(GL_QUADS);
    glVertex2f(m_origin.x - m_halfSize.x, m_origin.y - m_halfSize.y);
    glVertex2f(m_origin.x + m_halfSize.x, m_origin.y - m_halfSize.y);
    glVertex2f(m_origin.x + m_halfSize.x, m_origin.y + m_halfSize.y);
    glVertex2f(m_origin.x - m_halfSize.x, m_origin.y + m_halfSize.y);
    glEnd();
    
    glColor4f(0.0f, 0.0f, 0.0f, opacity);
    Vector2 position = m_origin - m_halfSize;
    position.y += 5;
    g_pFont->DrawLine(m_text, position);
}