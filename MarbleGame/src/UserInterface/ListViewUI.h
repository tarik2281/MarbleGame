//
//  ListViewUI.h
//  MarbleGame
//
//  Created by Tarik Karaca on 30.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__ListViewUI__
#define __MarbleGame__ListViewUI__

#include "ControlUI.h"
#include "Delegate.h"

#include <vector>

typedef Delegate<void, int> ListSelectDelegate;

class ListViewUI : public ControlUI {
public:
    bool HandleMButtonEvent(const MButtonInputData&);
    bool HandleMMotionEvent(const MMotionInputData&);
    bool HandleMScrollEvent(const MScrollInputData&);
    
    void Update();
    void Render(float alpha = 1.0f);
    
    std::vector<std::wstring> m_items;
    float m_scroll = 0.0f;
    int selectedIndex;
    ListSelectDelegate m_delegate;
};

typedef std::shared_ptr<ListViewUI> ListViewPtr;

#endif /* defined(__MarbleGame__ListViewUI__) */
