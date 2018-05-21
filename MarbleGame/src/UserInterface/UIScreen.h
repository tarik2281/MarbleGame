//
//  UIScreen.h
//  MarbleGame
//
//  Created by Tarik Karaca on 12.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__UIScreen__
#define __MarbleGame__UIScreen__

#include "ScreenManager.h"
#include "InputManager.h"
#include "ControlUI.h"
#include "EventManager.h"

#define TRANSITION_TIME 0.15f

enum TransitionState {
    TransitionOn,
    TransitionOff,
    TransitionNone
};

class ResourceManager;

class UIScreen : public IScreen, public IEventListener, public IMouseInputHandler, public IKeyInputHandler {
    friend class ControlUI;
protected:
    typedef std::vector<ControlPtr> ControlList;
    
    ControlList m_controls;
    ScreenPtr m_parent;
    TransitionState m_state;
    float m_opacity = 0.0f;
    static ResourceManager* s_resources;
public:
    UIScreen(const ScreenPtr& parent = nullptr);
    ~UIScreen();
    
    void Initialize();
    void Render();
    void Update();
    
    void OnAdded();
    void OnRemoved();
    void OnResize();
    
    void ExitScreen();
    
    bool HandleKeyEvent(const KeyInputData&);
    bool HandleMButtonEvent(const MButtonInputData&);
    bool HandleMMotionEvent(const MMotionInputData&);
    bool HandleMScrollEvent(const MScrollInputData&);
    
    void HandleEvent(const EventDataPtr&);
    
    void GoBack();
    void PushScreen(const ScreenPtr&);
    
    void AddControl(ControlPtr control) {
        control->SetOwner(this);
        const CSize& size = m_pScreenManager->GetScreenSize();
        control->OnResize(size.width, size.height);
        m_controls.push_back(control);
    }

    static void InitUI();
    static void ReleaseUI();
};

#endif /* defined(__MarbleGame__UIScreen__) */
