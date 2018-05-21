//
//  DialogBox.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 11.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "DialogBox.h"
#include "ButtonUI.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Font.h"
#include "Shader.h"

#include "Resource.h"

#include "Util.h"

#define DBOX_LEFT 97
#define DBOX_RIGHT 97
#define DBOX_TOP 33
#define DBOX_BOTTOM 12

void DialogBox::OnAdded() {
    UIScreen::OnAdded();
    
    g_pInputManager->SetSingleReceiver(this);
}

void DialogBox::OnRemoved() {
    UIScreen::OnRemoved();
    
    g_pInputManager->ReleaseSingleReceiver();
    m_delegate(m_result);
}

void DialogBox::Initialize() {
    CSize screenSize = m_pScreenManager->GetScreenSize();
    
    ButtonUI* yesButton = DBG_NEW ButtonUI;
    yesButton->SetOrigin(1.0f / 2.0f, 1.0f / 2.0f);
    yesButton->SetPixelPositionOffset(-50, 0);
    yesButton->SetText(L"Yes");
    yesButton->m_delegate = ButtonClickDelegate::MakeDelegate(DialogBox, YesHandler, this);
    yesButton->m_showTooltip = false;
    yesButton->m_background = s_resources->LoadContent<Texture2D>("button.png");
    yesButton->m_hoverBackground = s_resources->LoadContent<Texture2D>("buttonHover.png");
    AddControl(ControlPtr(yesButton));
    
    ButtonUI* noButton = DBG_NEW ButtonUI;
    noButton->SetOrigin(1.0f / 2.0f, 1.0f / 2.0f);
    noButton->SetPixelPositionOffset(50, 0);
    noButton->SetText(L"No");
    noButton->m_delegate = ButtonClickDelegate::MakeDelegate(DialogBox, NoHandler, this);
    noButton->m_showTooltip = false;
    noButton->m_background = s_resources->LoadContent<Texture2D>("button.png");
    noButton->m_hoverBackground = s_resources->LoadContent<Texture2D>("buttonHover.png");
    AddControl(ControlPtr(noButton));
    
    m_titleOrigin = Vector2(screenSize.width / 2, screenSize.height / 2 + 50);
    m_background = s_resources->LoadContent<Texture2D>("DialogBox.png");
    
    m_halfSize.x = 600.0f / 2.0f;
    m_halfSize.y = 140.0f / 2.0f;
}

void DialogBox::Release() {
    
}

void DialogBox::Render() {
    Shader::Unbind();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_pScreenManager->getOrthoMatrix().m);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, m_opacity / 1.5f);
    glVertex2f(0, 0);
    glVertex2f(1920, 0);
    glVertex2f(1920, 1080);
    glVertex2f(0, 1080);
    glEnd();
    
    
    
    glColor4f(1.0f, 1.0f, 1.0f, m_opacity);
    Vector2 origin(m_pScreenManager->GetScreenSize().width / 2.0f, m_pScreenManager->GetScreenSize().height / 2.0f);
    
    m_background->Bind(0);
    
    float texLeft = DBOX_LEFT / float(m_background->getWidth());
    float texRight = (m_background->getWidth() - DBOX_RIGHT) / float(m_background->getWidth());
    float texBottom = DBOX_BOTTOM / float(m_background->getHeight());
    float texTop = (m_background->getHeight() - DBOX_TOP) / float(m_background->getHeight());
    
    int vertCLeft = origin.x - m_halfSize.x - MARGIN;
    int vertLeft = origin.x - m_halfSize.x + DBOX_LEFT - MARGIN;
    int vertCRight = origin.x + m_halfSize.x + MARGIN;
    int vertRight = origin.x + m_halfSize.x - DBOX_RIGHT + MARGIN;
    
    int vertCBottom = origin.y - m_halfSize.y - MARGIN;
    int vertBottom = origin.y - m_halfSize.y + DBOX_BOTTOM - MARGIN;
    int vertCTop = origin.y + m_halfSize.y + MARGIN;
    int vertTop = origin.y + m_halfSize.y - DBOX_TOP + MARGIN;
    
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

    
    UIScreen::Render();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(m_pScreenManager->getOrthoMatrix().m);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(1.0f, 1.0f, 1.0f, m_opacity);
    g_pFont->DrawLine(m_title, m_titleOrigin - m_titleHalfSize);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
}

void DialogBox::YesHandler() {
    m_result = DIALOG_RESULT_YES;
    ExitScreen();
}

void DialogBox::NoHandler() {
    m_result = DIALOG_RESULT_NO;
    ExitScreen();
}

void DialogBox::SetTitle(const std::wstring &title) {
    m_title = title;
    m_titleHalfSize.x = 14.0f * title.length() / 2.0f;
    m_titleHalfSize.y = 27.0f / 2.0f;
}