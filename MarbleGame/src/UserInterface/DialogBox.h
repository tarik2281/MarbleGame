//
//  DialogBox.h
//  MarbleGame
//
//  Created by Tarik Karaca on 11.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__DialogBox__
#define __MarbleGame__DialogBox__

#include "ScreenManager.h"
#include "UIScreen.h"
#include "Delegate.h"

#include <string>

class Texture2D;

typedef Delegate<void, int> DialogResultDelegate;

#define DIALOG_RESULT_YES 1
#define DIALOG_RESULT_NO 2

class DialogBox : public UIScreen {
    std::wstring m_title;
    Vector2 m_titleHalfSize;
    std::wstring m_description;
    int m_result;
    Vector2 m_titleOrigin;
    Texture2D* m_background = 0;
    Vector2 m_halfSize;
public:
    DialogBox() : UIScreen() { }
    ~DialogBox() { }
    
    void OnAdded();
    void OnRemoved();
    void Initialize();
    void Release();
    void Render();
    
    void YesHandler();
    void NoHandler();
    
    void SetTitle(const std::wstring&);
    
    DialogResultDelegate m_delegate;
};

#endif /* defined(__MarbleGame__DialogBox__) */
