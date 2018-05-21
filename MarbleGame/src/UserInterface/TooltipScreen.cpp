//
//  TooltipScreen.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 30.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "TooltipScreen.h"

#include "Renderer.h"
#include "Font.h"
#include "Shader.h"

#include "Util.h"

void TooltipScreen::Render() {
    Shader::Unbind();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_pScreenManager->getOrthoMatrix().m);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glColor4f(1.0f, 0.8f, 0.3f, m_opacity);
    glBegin(GL_QUADS);
    glVertex2f(m_origin.x - m_size.x / 2 - 5, m_origin.y - m_size.y / 2 - 5);
    glVertex2f(m_origin.x + m_size.x / 2 + 5, m_origin.y - m_size.y / 2 - 5);
    glVertex2f(m_origin.x + m_size.x / 2 + 5, m_origin.y + m_size.y / 2 + 5);
    glVertex2f(m_origin.x - m_size.x / 2 - 5, m_origin.y + m_size.y / 2 + 5);
    glEnd();
    
    glColor4f(0.0f, 0.0f, 0.0f, m_opacity);
    Vector2 position = m_origin;
    position.x -= m_size.x / 2;
    position.y -= m_size.y / 2;
    g_pFont->DrawLine(m_text, position);
}

void TooltipScreen::Initialize() {
    m_size = g_pFont->GetSize(m_text);
}