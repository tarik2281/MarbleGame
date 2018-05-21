//
//  ListViewUI.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 30.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "ListViewUI.h"

#include "Renderer.h"
#include "Font.h"
#include "InputData.h"

#include "Util.h"

bool ListViewUI::HandleMButtonEvent(const MButtonInputData &data) {
    
    if (data.m_button == MButtonLeft && data.m_state) {
        if (selectedIndex != -1) {
            m_delegate(selectedIndex);
            return true;
        }
    }
    
    return false;
}

bool ListViewUI::HandleMMotionEvent(const MMotionInputData &data) {
    Vector2 leftBottom = m_absoluteOrigin - m_halfSize;
    Vector2 rightTop = m_absoluteOrigin + m_halfSize;
   
    if (data.m_x < leftBottom.x || data.m_x > rightTop.x ||
        data.m_y < leftBottom.y || data.m_y > rightTop.y) {
        selectedIndex = -1;
        return false;
    }
    
    selectedIndex = floor((2.0f * m_halfSize.y - (data.m_y - (m_absoluteOrigin.y - m_halfSize.y) + 10 + m_scroll)) / 32);
    
    return true;
}

bool ListViewUI::HandleMScrollEvent(const MScrollInputData &data) {
    if (float(m_items.size()) * 32 < m_halfSize.y * 2.0f)
        return false;
    
    m_scroll += data.m_deltaY * 4.0f;
    if (m_scroll > 0)
        m_scroll = 0;
    if (m_scroll < -(float(m_items.size()) * 32) + m_halfSize.y * 2.0f)
        m_scroll = -(float(m_items.size()) * 32) + m_halfSize.y * 2.0f;
    return false;
}

void ListViewUI::Update() {
    
}

void ListViewUI::Render(float opacity) {
    Vector4 backgroundColor(1.0f, 1.0f, 1.0f, opacity);
    Vector4 hoverColor(0.2f, 0.4f, 0.8f, opacity);
    Vector4 textColor(0.0f, 0.0f, 0.0f, opacity);
    
    glScissor(m_absoluteOrigin.x - m_halfSize.x, m_absoluteOrigin.y - m_halfSize.y, m_halfSize.x * 2.0f, m_halfSize.y * 2.0f);
    glEnable(GL_SCISSOR_TEST);
    
    
    glColor4fv(backgroundColor.v);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
    glVertex2fv((m_absoluteOrigin - m_halfSize).v);
    glVertex2f(m_absoluteOrigin.x + m_halfSize.x, m_absoluteOrigin.y - m_halfSize.y);
    glVertex2fv((m_absoluteOrigin + m_halfSize).v);
    glVertex2f(m_absoluteOrigin.x - m_halfSize.x, m_absoluteOrigin.y + m_halfSize.y);
    glEnd();
    
    Vector2 position = m_absoluteOrigin;
    position.x -= m_halfSize.x;
    position.y += m_halfSize.y;
    position.y -= m_scroll;
    
    int index = floor(fabsf(m_scroll) / 32);
    int endIndex = ceil(m_halfSize.y * 2.0f / 32) + 1;
    endIndex = fminf(endIndex, m_items.size());
    endIndex += index;
    endIndex = fminf(endIndex, m_items.size());
    
    for (int i = index; i < endIndex; i++) {
        Vector2 pos = position + Vector2(3.0f, -(i+1) * 32);
        std::wstring& item = m_items[i];
        
        glColor4fv((i == selectedIndex ? hoverColor : textColor).v);
        g_pFont->DrawLine(item, pos);
    }
    glDisable(GL_SCISSOR_TEST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor4fv(textColor.v);
    
    float height = float(m_items.size()) * 32.0f;
    float part = (m_halfSize.y * 2.0f) / height;
    float y = part * (m_halfSize.y * 2.0f);
    float offset = fabsf(m_scroll) / height;
    offset = offset * (m_halfSize.y * 2.0f);
    glBegin(GL_QUADS);
    glVertex2f(m_absoluteOrigin.x + m_halfSize.x - 12, m_absoluteOrigin.y + m_halfSize.y - y - offset);
    glVertex2f(m_absoluteOrigin.x + m_halfSize.x - 2, m_absoluteOrigin.y + m_halfSize.y - y - offset);
    glVertex2f(m_absoluteOrigin.x + m_halfSize.x - 2, m_absoluteOrigin.y + m_halfSize.y - offset);
    glVertex2f(m_absoluteOrigin.x + m_halfSize.x - 12, m_absoluteOrigin.y + m_halfSize.y - offset);
    glEnd();
}