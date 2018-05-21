//
//  MenuScreen.h
//  MarbleGame
//
//  Created by Tarik Karaca on 08.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__MenuScreen__
#define __MarbleGame__MenuScreen__

#include "ScreenManager.h"
#include "InputManager.h"
#include "ButtonUI.h"
#include "UIScreen.h"
#include "ListViewUI.h"

class MenuScreen : public UIScreen {
    ListViewPtr m_list;
    std::wstring text;
public:
    MenuScreen() : UIScreen() { }
    ~MenuScreen() { Release(); }
    
    void Initialize();
    void Release();
    void Render();
    
    void PlayHandler();
    void SettingsHandler();
    void ExitHandler();
    
    void ResultHandler(int);
    void SelectHandler(int);
};

class SubMenuScreen : public UIScreen {
public:
    SubMenuScreen(const ScreenPtr& parent) : UIScreen(parent) { }
    ~SubMenuScreen() { Release(); }
    
    void Initialize();
    void Release();
    void Render();
};

class PauseScreen : public UIScreen {
    enum Result {
        ResultResume,
        ResultExit
    };
    
    int m_result;
    CSize m_screenSize;
public:
    PauseScreen() : UIScreen(nullptr) { }
    ~PauseScreen() { Release(); }
    
    void Initialize();
    void Release();
    void Render();
    
    void OnAdded();
    void OnRemoved();
    
    void ResumeHandler();
    void ExitHandler();
    
    bool HandleKeyEvent(const KeyInputData&);
};

#endif /* defined(__MarbleGame__MenuScreen__) */
