//
//  ButtonUI.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 09.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "ButtonUI.h"

#include "InputData.h"
#include "Renderer.h"
#include "Font.h"
#include "TooltipScreen.h"
#include "ScreenManager.h"
#include "Shader.h"

#include "Util.h"

void ButtonUI::Render(float alpha) {
    Shader::Unbind();
    Vector4 color(1.0f, 1.0f, 1.0f, alpha);
    if (m_hover && !m_background)
        color = Vector4(0.3f, 0.6f, 1.0f, alpha);
    
    if (m_background || m_hoverBackground) {
        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        if (!m_hover)
            m_background->Bind(0);
        else
            m_hoverBackground->Bind(0);
        
        float texLeft = LEFT / float(m_background->getWidth());
        float texRight = (m_background->getWidth() - RIGHT) / float(m_background->getWidth());
        float texBottom = BOTTOM / float(m_background->getHeight());
        float texTop = (m_background->getHeight() - TOP) / float(m_background->getHeight());
        
        int vertCLeft = m_absoluteOrigin.x - m_halfSize.x - MARGIN;
        int vertLeft = m_absoluteOrigin.x - m_halfSize.x + LEFT - MARGIN;
        int vertCRight = m_absoluteOrigin.x + m_halfSize.x + MARGIN;
        int vertRight = m_absoluteOrigin.x + m_halfSize.x - RIGHT + MARGIN;
        
        int vertCBottom = m_absoluteOrigin.y - m_halfSize.y - MARGIN;
        int vertBottom = m_absoluteOrigin.y - m_halfSize.y + BOTTOM - MARGIN;
        int vertCTop = m_absoluteOrigin.y + m_halfSize.y + MARGIN;
        int vertTop = m_absoluteOrigin.y + m_halfSize.y - TOP + MARGIN;
        
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(vertCLeft, vertCBottom);
        glTexCoord2f(texLeft, 0.0f);
        glVertex2f(vertLeft, vertCBottom);
        glTexCoord2f(texLeft, texBottom);
        glVertex2f(vertLeft, vertBottom);
        glTexCoord2f(0.0f, texBottom);
        glVertex2f(vertCLeft, vertBottom);
        
        glTexCoord2f(texLeft, 0.0f);
        glVertex2f(vertLeft, vertCBottom);
        glTexCoord2f(texRight, 0.0f);
        glVertex2f(vertRight, vertCBottom);
        glTexCoord2f(texRight, texBottom);
        glVertex2f(vertRight, vertBottom);
        glTexCoord2f(texLeft, texBottom);
        glVertex2f(vertLeft, vertBottom);
        
        glTexCoord2f(texRight, 0.0f);
        glVertex2f(vertRight, vertCBottom);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(vertCRight, vertCBottom);
        glTexCoord2f(1.0f, texBottom);
        glVertex2f(vertCRight, vertBottom);
        glTexCoord2f(texRight, texBottom);
        glVertex2f(vertRight, vertBottom);
        
        
        glTexCoord2f(0.0f, texBottom);
        glVertex2f(vertCLeft, vertBottom);
        glTexCoord2f(texLeft, texBottom);
        glVertex2f(vertLeft, vertBottom);
        glTexCoord2f(texLeft, texTop);
        glVertex2f(vertLeft, vertTop);
        glTexCoord2f(0.0f, texTop);
        glVertex2f(vertCLeft, vertTop);
        
        glTexCoord2f(texLeft, texBottom);
        glVertex2f(vertLeft, vertBottom);
        glTexCoord2f(texRight, texBottom);
        glVertex2f(vertRight, vertBottom);
        glTexCoord2f(texRight, texTop);
        glVertex2f(vertRight, vertTop);
        glTexCoord2f(texLeft, texTop);
        glVertex2f(vertLeft, vertTop);
        
        glTexCoord2f(texRight, texBottom);
        glVertex2f(vertRight, vertBottom);
        glTexCoord2f(1.0f, texBottom);
        glVertex2f(vertCRight, vertBottom);
        glTexCoord2f(1.0f, texTop);
        glVertex2f(vertCRight, vertTop);
        glTexCoord2f(texRight, texTop);
        glVertex2f(vertRight, vertTop);
        
        
        glTexCoord2f(0.0f, texTop);
        glVertex2f(vertCLeft, vertTop);
        glTexCoord2f(texLeft, texTop);
        glVertex2f(vertLeft, vertTop);
        glTexCoord2f(texLeft, 1.0f);
        glVertex2f(vertLeft, vertCTop);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(vertCLeft, vertCTop);
        
        glTexCoord2f(texLeft, texTop);
        glVertex2f(vertLeft, vertTop);
        glTexCoord2f(texRight, texTop);
        glVertex2f(vertRight, vertTop);
        glTexCoord2f(texRight, 1.0f);
        glVertex2f(vertRight, vertCTop);
        glTexCoord2f(texLeft, 1.0f);
        glVertex2f(vertLeft, vertCTop);
        
        glTexCoord2f(texRight, texTop);
        glVertex2f(vertRight, vertTop);
        glTexCoord2f(1.0f, texTop);
        glVertex2f(vertCRight, vertTop);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(vertCRight, vertCTop);
        glTexCoord2f(texRight, 1.0f);
        glVertex2f(vertRight, vertCTop);
        glEnd();
        
    }
    
    glColor4fv(color.v);
    //Vector2 temp = g_pFont->GetSize(m_text) / 2;
    g_pFont->DrawLine(m_text, m_absoluteOrigin - m_halfSize);
}

bool ButtonUI::HandleMButtonEvent(const MButtonInputData &data) {
    if (m_hover && data.m_button == 0 && data.m_state) {
        m_hover = false;

        if (m_tooltip && m_showTooltip)
            m_tooltip->ExitScreen();
        
        m_delegate();
        return true;
    }
    return false;
}

bool ButtonUI::HandleMMotionEvent(const MMotionInputData &data) {
    bool temp = m_hover;
    m_hover = (data.m_x > m_absoluteOrigin.x - m_halfSize.x - MARGIN && data.m_x < m_absoluteOrigin.x + m_halfSize.x + MARGIN &&
               data.m_y > m_absoluteOrigin.y - m_halfSize.y - MARGIN && data.m_y < m_absoluteOrigin.y + m_halfSize.y + MARGIN);
    if (m_showTooltip) {
        if (temp != m_hover) {
            if (m_hover) {
                if (!m_tooltip) {
                    m_tooltip = ScreenPtr(DBG_NEW TooltipScreen(m_text, m_absoluteOrigin + Vector2(m_halfSize.x, 40)));
                }
            
                g_pScreenManager->AddScreen(m_tooltip);
            } else {
                if (m_tooltip)
                    m_tooltip->ExitScreen();
            }
        }
    }
    return m_hover;
}

void ButtonUI::SetText(const std::wstring &text) {
    m_text = text;
    m_halfSize = g_pFont->GetSize(text);
    m_halfSize.x /= 2.0f;
    m_halfSize.y /= 2.0f;
}