//
//  LabelUI.h
//  MarbleGame
//
//  Created by Tarik Karaca on 12.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__LabelUI__
#define __MarbleGame__LabelUI__

#include "ControlUI.h"
#include <string>

class LabelUI : public ControlUI {
    std::wstring m_text;
public:
    void Render(float);
    
    void SetText(const std::wstring&);
};

typedef std::shared_ptr<LabelUI> LabelPtr;

#endif /* defined(__MarbleGame__LabelUI__) */
