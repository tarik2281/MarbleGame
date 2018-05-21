//
//  ControlUI.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 30.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "ControlUI.h"

#include "UIScreen.h"

void ControlUI::SetAbsolutePosition(int width, int height) {
    Vector2 absPos;
    if (m_origin.x == 0.0f) {
        absPos.x = m_halfSize.x;
    } else if (m_origin.x == 1.0f) {
        absPos.x = width - m_halfSize.x;
    } else {
        absPos.x = m_origin.x * width;
    }
    
    if (m_origin.y == 0.0f) {
        absPos.y = m_halfSize.y;
    } else if (m_origin.y == 1.0f) {
        absPos.y = height - m_halfSize.y;
    } else {
        absPos.y = m_origin.y * height;
    }
    
    m_absoluteOrigin = absPos + m_pixelPositionOffset;
}

bool ContainerUI::HandleMButtonEvent(const MButtonInputData &data) {
    ControlList::reverse_iterator it;
    
    for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
        if ((*it)->HandleMButtonEvent(data))
            return true;
    }
    
    return false;
}

bool ContainerUI::HandleMMotionEvent(const MMotionInputData &data) {
    ControlList::reverse_iterator it;
    
    for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
        if ((*it)->HandleMMotionEvent(data))
            return true;
    }
    
    return false;
}

bool ContainerUI::HandleMScrollEvent(const MScrollInputData &data) {
    ControlList::reverse_iterator it;
    
    for (it = m_controls.rbegin(); it != m_controls.rend(); ++it) {
        if ((*it)->HandleMScrollEvent(data))
            return true;
    }
    
    return false;
}

void ContainerUI::Render(float opacity) {
    ControlList::iterator it;
    
    for (it = m_controls.begin(); it != m_controls.end(); ++it) {
        (*it)->Render(opacity);
    }
}