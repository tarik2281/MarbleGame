//
//  ScreenManager.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 07.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "ScreenManager.h"
#include "EventManager/Events.h"
#include "Util.h"
#include "main.h"

void ScreenManager::AddScreen(const ScreenPtr& screen) {
    if (!screen)
        return;
    
    screen->SetScreenManager(this);
    screen->OnAdded();
    screen->m_isDone = false;
    m_screens.push_back(screen);
}

void ScreenManager::RemoveScreen(const ScreenPtr& screen) {
    if (!screen)
        return;
    
    screen->OnRemoved();
    m_screens.remove(screen);
}

void ScreenManager::Render() {
    ScreenList::iterator it;
    for (it = m_screens.begin(); it != m_screens.end(); ++it) {
        (*it)->Render();
    }
}

void ScreenManager::SwitchToScreen(const ScreenPtr &screen) {
    ScreenList().swap(m_screens);
    m_screens.push_back(screen);
}

void ScreenManager::Update() {
    ScreenList::iterator it = m_screens.begin();
    
    while (it != m_screens.end()) {
        (*it)->Update();
        
        if ((*it)->m_isDone) {
            (*it)->OnRemoved();
            m_screens.erase(it++);
        } else {
            ++it;
        }
    }
}

void ScreenManager::Initialize(int screenWidth, int screenHeight) {
    m_screenSize.width = screenWidth;
    m_screenSize.height = screenHeight;
    
    m_orthoMatrix = MatrixMakeOrtho(0, screenWidth, 0, screenHeight, 1, -1);
    
    g_pEventManager->AddEventListener(this, ContextResizeEvent::sk_eventType);
}

void ScreenManager::Release() {
    g_pEventManager->RemoveEventListener(this, ContextResizeEvent::sk_eventType);
}

void ScreenManager::HandleEvent(const EventDataPtr &data) {
    if (data->getEventType() == ContextResizeEvent::sk_eventType) {
        ContextResizeEvent* event = static_cast<ContextResizeEvent*>(data.get());
        m_screenSize.width = event->m_screenWidth;
        m_screenSize.height = event->m_screenHeight;
        
        m_orthoMatrix = MatrixMakeOrtho(0, event->m_screenWidth, 0, event->m_screenHeight, 1, -1);
    }
}
