//
//  MenuScreen.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 08.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "MenuScreen.h"
#include "Util.h"
#include "Renderer.h"
#include "GameplayScreen.h"
#include "ButtonUI.h"
#include "Font.h"
#include "ResourceManager.h"
#include "DialogBox.h"
#include "EventManager.h"
#include "TextBoxUI.h"

void MenuScreen::Initialize() {
    UIScreen::Initialize();
    
    m_inputMask |= MSCROLL_MASK;
    
    CSize screenSize = m_pScreenManager->GetScreenSize();
    
    ButtonUI* playButton = DBG_NEW ButtonUI;
    playButton->SetOrigin(1.0f / 4.0f, 1.0f / 2.0f);
    playButton->SetPixelPositionOffset(0, 45);
    playButton->m_delegate = ButtonClickDelegate::MakeDelegate(MenuScreen, PlayHandler, this);
    playButton->SetText(L"Play Game");
    AddControl(ButtonPtr(playButton));
    
    ButtonUI* settingsButton = DBG_NEW ButtonUI;
    settingsButton->SetOrigin(1.0f / 4.0f, 1.0f / 2.0f);
    settingsButton->m_delegate = ButtonClickDelegate::MakeDelegate(MenuScreen, SettingsHandler, this);
    settingsButton->SetText(L"Settings");
    AddControl(ButtonPtr(settingsButton));
    
    ButtonUI* exitButton = DBG_NEW ButtonUI;
    exitButton->SetOrigin(1.0f / 4.0f, 1.0f / 2.0f);
    exitButton->SetPixelPositionOffset(0, -45);
    exitButton->m_delegate = ButtonClickDelegate::MakeDelegate(MenuScreen, ExitHandler, this);
    exitButton->SetText(L"Exit");
    AddControl(ButtonPtr(exitButton));
    
    m_list = ListViewPtr(DBG_NEW ListViewUI);
    m_list->SetOrigin(1.0f / 2.0f, 1.0f / 2.0f);
    m_list->SetHalfSize(screenSize.width / 8.0f, screenSize.height / 8.0f);
    m_list->m_items.push_back(L"Test Item");
    m_list->m_items.push_back(L"Another Item");
    m_list->m_items.push_back(L"Motherfucker");
    m_list->m_items.push_back(L"Shit");
    m_list->m_items.push_back(L"Bitch");
    m_list->m_items.push_back(L"Annen");
    m_list->m_items.push_back(L"Baban");
    m_list->m_delegate = ListSelectDelegate::MakeDelegate(MenuScreen, SelectHandler, this);
    //AddControl(m_list);
    
    /*TextBoxUI* textBox = DBG_NEW TextBoxUI;
    textBox->SetOrigin(Vector2(500, 50));
    textBox->SetHalfSize(200, 15);
    AddControl(ControlPtr(textBox));*/
}

void MenuScreen::Release() {
    
}

void MenuScreen::PlayHandler() {
    GameplayScreen* sub = DBG_NEW GameplayScreen();
    m_pScreenManager->AddScreen(ScreenPtr(sub));
    //TimeScreen* scr = DBG_NEW TimeScreen();
    //m_pScreenManager->AddScreen(ScreenPtr(scr));
    GameMenuScreen* menu = DBG_NEW GameMenuScreen();
    m_pScreenManager->AddScreen(ScreenPtr(menu));
    ExitScreen();
    //m_pScreenManager->RemoveScreen(shared_from_this());
}

void MenuScreen::SettingsHandler() {
    SubMenuScreen* sub = DBG_NEW SubMenuScreen(shared_from_this());
    m_pScreenManager->AddScreen(ScreenPtr(sub));
    //m_pScreenManager->RemoveScreen(shared_from_this());
    ExitScreen();
}

void MenuScreen::ExitHandler() {
    DialogBox* dBox = DBG_NEW DialogBox();
    dBox->m_delegate = DialogResultDelegate::MakeDelegate(MenuScreen, ResultHandler, this);
    m_pScreenManager->AddScreen(ScreenPtr(dBox));
    dBox->SetTitle(L"Exit?");
    //ExitScreen();
}

void MenuScreen::Render() {
    UIScreen::Render();
    
    Vector2 position;
    Vector2 size = g_pFont->GetSize(text);
    position.x = m_pScreenManager->GetScreenSize().width - size.x;
    position.y = 0;
    glColor4f(1.0f, 1.0f, 1.0f, m_opacity);
    g_pFont->DrawLine(text, position);
}

void MenuScreen::ResultHandler(int result) {
    if (result == DIALOG_RESULT_YES)
        quitGame();
}

void MenuScreen::SelectHandler(int selectedIndex) {
    text = m_list->m_items[selectedIndex];
}


void SubMenuScreen::Initialize() {
    UIScreen::Initialize();
    
    CSize screenSize = m_pScreenManager->GetScreenSize();
    
    ButtonUI* backButton = DBG_NEW ButtonUI;
    backButton->SetOrigin(1.0f / 4.0f, 1.0f / 2.0f);
    backButton->m_delegate = ButtonClickDelegate::MakeDelegate(UIScreen, GoBack, this);
    backButton->SetText(L"Back");
    AddControl(ButtonPtr(backButton));
}

void SubMenuScreen::Release() {
    
}

void SubMenuScreen::Render() {
    UIScreen::Render();
}




void PauseScreen::Initialize() {
    UIScreen::Initialize();
    
    m_screenSize = m_pScreenManager->GetScreenSize();
    
    ButtonUI* resumeButton = DBG_NEW ButtonUI;
    resumeButton->SetOrigin(1.0f / 2.0f, 1.0f / 2.0f);
    resumeButton->m_delegate = ButtonClickDelegate::MakeDelegate(PauseScreen, ResumeHandler, this);
    resumeButton->SetText(L"Resume");
    resumeButton->m_background = s_resources->LoadContent<Texture2D>("button.png");
    resumeButton->m_hoverBackground = s_resources->LoadContent<Texture2D>("buttonHover.png");
    AddControl(ButtonPtr(resumeButton));
    
    ButtonUI* exitButton = DBG_NEW ButtonUI;
    exitButton->SetOrigin(1.0f / 2.0f, 1.0f / 2.0f);
    exitButton->SetPixelPositionOffset(0, -50);
    exitButton->m_delegate = ButtonClickDelegate::MakeDelegate(PauseScreen, ExitHandler, this);
    exitButton->SetText(L"Exit");
    exitButton->SetHalfSize(resumeButton->GetHalfSize());
    exitButton->m_background = s_resources->LoadContent<Texture2D>("button.png");
    exitButton->m_hoverBackground = s_resources->LoadContent<Texture2D>("buttonHover.png");
    AddControl(ButtonPtr(exitButton));
}

void PauseScreen::Release() {
    switch (m_result) {
        case ResultResume:
            
            break;
        case ResultExit: {
            GameExitEvent* event = DBG_NEW GameExitEvent;
            g_pEventManager->QueueEvent(EventDataPtr(event));
            MenuScreen* scr = DBG_NEW MenuScreen;
            m_pScreenManager->AddScreen(ScreenPtr(scr));
            break;
        }
        default:
            break;
    }

}

void PauseScreen::Render() {
    Shader::Unbind();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_pScreenManager->getOrthoMatrix().m);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const CSize& size = m_pScreenManager->GetScreenSize();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, m_opacity / 2.0f);
    glVertex2f(0, 0);
    glVertex2f(size.width, 0);
    glVertex2f(size.width, size.height);
    glVertex2f(0, size.height);
    glEnd();
    
    UIScreen::Render();
}

void PauseScreen::OnAdded() {
    UIScreen::OnAdded();
    
    g_pInputManager->AddKeyInputHandler(HotKey::k_Wildcard, this);
    g_pInputManager->SetSingleReceiver(this);
    GamePausedEvent* event = DBG_NEW GamePausedEvent(true);
    g_pEventManager->QueueEvent(EventDataPtr(event));
}

void PauseScreen::OnRemoved() {
    UIScreen::OnRemoved();
    
    g_pInputManager->RemoveKeyInputHandler(HotKey::k_Wildcard);
    g_pInputManager->ReleaseSingleReceiver();
    GamePausedEvent* event = DBG_NEW GamePausedEvent(false);
    g_pEventManager->QueueEvent(EventDataPtr(event));
}

bool PauseScreen::HandleKeyEvent(const KeyInputData &data) {
    if (data.m_keyCode == 27 && data.m_state) {
        m_result = ResultResume;
        ExitScreen();
    }
    return false;
}

void PauseScreen::ResumeHandler() {
    m_result = ResultResume;
    ExitScreen();
}

void PauseScreen::ExitHandler() {
    m_result = ResultExit;
    ExitScreen();
}
