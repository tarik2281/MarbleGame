//
//  LabelUI.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 12.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "LabelUI.h"

#include "Renderer.h"
#include "Font.h"

#include "Util.h"

void LabelUI::SetText(const std::wstring &text) {
    m_text = text;
    m_halfSize = g_pFont->GetSize(text);
    m_halfSize /= 2.0f;
}

void LabelUI::Render(float opacity) {
    glColor4f(1.0f, 1.0f, 1.0f, opacity);
    g_pFont->DrawLine(m_text, m_origin);
}