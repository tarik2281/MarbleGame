//
//  UIScreen.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 12.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "UIScreen.h"

#include "Shader.h"
#include "Renderer.h"
#include "Font.h"

#include "ResourceManager.h"

#include "Util.h"

ResourceManager* UIScreen::s_resources = 0;

UIScreen::UIScreen(const ScreenPtr& parent) : m_parent(parent), m_state(TransitionOn) {
    g_pEventManager->AddEventListener(this, ContextResizeEvent::sk_eventType);
}

UIScreen::~UIScreen() {
    g_pEventManager->RemoveEventListener(this, ContextResizeEvent::sk_eventType);
}

void UIScreen::Initialize() {
    UIScreen::InitUI();
    
    m_inputMask |= MSCROLL_MASK;
}

void UIScreen::Render() {
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
    
    ControlList::iterator it;
    for (it = m_controls.begin(); it != m_controls.end(); it++) {
        (*it)->Render(m_opacity);
    }
}

void UIScreen::Update() {
    if (m_state == TransitionOn) {
        float step = 1.0f * ((1.0f / 60.0f) / TRANSITION_TIME);
        
        m_opacity += step;
        if (m_opacity >= 1.0f)
            m_state = TransitionNone;
    } else if (m_state == TransitionOff) {
        float step = 1.0f * ((1.0f / 60.0f) / TRANSITION_TIME);
        
        m_opacity -= step;
        if (m_opacity <= 0.0f)
            m_state = TransitionNone;
        
        if (m_state == TransitionNone) {
            IScreen::ExitScreen();
            m_state = TransitionOn;
        }
    }
}

void UIScreen::GoBack() {
    if (m_parent) {
        m_pScreenManager->AddScreen(m_parent);
        //m_parent->Initialize();
        ExitScreen();
        //m_pScreenManager->RemoveScreen(shared_from_this());
    }
}

bool UIScreen::HandleKeyEvent(const KeyInputData &data) {
    ControlList::reverse_iterator it;
    
    for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
        if ((*it)->HandleKeyEvent(data))
            return true;
    }
    
    return false;
}

bool UIScreen::HandleMButtonEvent(const MButtonInputData &data) {
    if (m_state != TransitionOff) {
        ControlList::reverse_iterator it;
    
        for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
            if ((*it)->HandleMButtonEvent(data))
                return true;
        }
    }
    
    return false;
}

bool UIScreen::HandleMMotionEvent(const MMotionInputData &data) {
    if (m_state != TransitionOff) {
        ControlList::reverse_iterator it;
    
        for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
            if ((*it)->HandleMMotionEvent(data))
                return true;
        }
    }
    
    return false;
}

bool UIScreen::HandleMScrollEvent(const MScrollInputData &data) {
    if (m_state != TransitionOff) {
        ControlList::reverse_iterator it;
        
        for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
            if ((*it)->HandleMScrollEvent(data))
                return true;
        }
    }
    
    return false;
}

void UIScreen::OnAdded() {
    IScreen::OnAdded();
    
    m_state = TransitionOn;
    g_pInputManager->AddMouseInputHandler(this);
}

void UIScreen::OnRemoved() {
    g_pInputManager->RemoveMouseInputHandler(this);
}

void UIScreen::HandleEvent(const EventDataPtr &data) {
    if (data->getEventType() == ContextResizeEvent::sk_eventType) {
        ContextResizeEvent* event = static_cast<ContextResizeEvent*>(data.get());
        
        ControlList::iterator it;
        for (it = m_controls.begin(); it != m_controls.end(); ++it) {
            (*it)->OnResize(event->m_screenWidth, event->m_screenHeight);
        }
    }
}

void UIScreen::ExitScreen() {
    m_state = TransitionOff;
}

void UIScreen::InitUI() {
    if (!s_resources) {
        s_resources = DBG_NEW ResourceManager;
        g_pFont = s_resources->LoadContent<Font>("Arial.ttf");
    }
}

void UIScreen::ReleaseUI() {
    s_resources->unloadAll();
    delete s_resources;
    s_resources = 0;
    g_pFont = 0;
}
