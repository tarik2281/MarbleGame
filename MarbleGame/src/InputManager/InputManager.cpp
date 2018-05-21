//
//  InputManager.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "InputManager.h"

const HotKey HotKey::k_Wildcard = HotKey(0xFF, 0xFFFFFFFF);

void InputManager::AddKeyInputHandler(HotKey key, IKeyInputHandler *handler) {
    if (!handler)
        return;
    
    if (key == HotKey::k_Wildcard)
        m_keyWildcardInList = true;
    
    m_hotKeys.insert(HotKeyPair(key, handler));
}

void InputManager::RemoveKeyInputHandler(HotKey key) {
    if (key == HotKey::k_Wildcard)
        m_keyWildcardInList = false;
    
    m_hotKeys.erase(key);
}

void InputManager::PollInputEvents() {
    m_processQueue = m_activeQueue;
    m_activeQueue = (m_activeQueue + 1) % 2;
    
    while (!m_removeQueue.empty()) {
        m_mouseHandlers.remove(m_removeQueue.front());
        m_removeQueue.pop();
    }
    
    InputEventQueue& q = m_queues[m_processQueue];
    while (!q.empty()) {
        const IInputDataPtr& data = q.front();
        switch (data->m_inputType) {
            case InputTypesKey: {
                KeyInputData* dat = static_cast<KeyInputData*>(data.get());
                HotKey key;
                if (m_keyWildcardInList)
                    key = HotKey::k_Wildcard;
                else {
                    key.key = dat->m_keyCode;
                    key.modifiers = dat->m_modifierFlags;
                }
                HotKeyMap::iterator it = m_hotKeys.find(key);
                if (it != m_hotKeys.end()) {
                    it->second->HandleKeyEvent(*dat);
                }
                break;
            }
                
            case InputTypesMButton: {
                MButtonInputData* dat = static_cast<MButtonInputData*>(data.get());
                
                if (m_singleReceiver) {
                    if (m_singleReceiver->getInputMask() & MBUTTON_MASK)
                        m_singleReceiver->HandleMButtonEvent(*dat);
                    break;
                }
                
                MouseHandlerList::reverse_iterator it;
                for (it = m_mouseHandlers.rbegin(); it != m_mouseHandlers.rend(); ++it) {
                    if (!(*it))
                        continue;
                    
                    if ((*it)->getInputMask() & MBUTTON_MASK)
                        if ((*it)->HandleMButtonEvent(*dat))
                            break;
                }
                break;
            }
                
            case InputTypesMMotion: {
                MMotionInputData* dat = static_cast<MMotionInputData*>(data.get());
                
                if (m_singleReceiver) {
                    if (m_singleReceiver->getInputMask() & MMOTION_MASK)
                        m_singleReceiver->HandleMMotionEvent(*dat);
                    break;
                }
                
                MouseHandlerList::reverse_iterator it;
                for (it = m_mouseHandlers.rbegin(); it != m_mouseHandlers.rend(); ++it) {
                    if (!(*it))
                        continue;
                    
                    if ((*it)->getInputMask() & MMOTION_MASK)
                        (*it)->HandleMMotionEvent(*dat);
                }
                break;
            }
                
            case InputTypesMScroll: {
                MScrollInputData* dat = static_cast<MScrollInputData*>(data.get());
                
                if (m_singleReceiver) {
                    if (m_singleReceiver->getInputMask() & MSCROLL_MASK)
                        m_singleReceiver->HandleMScrollEvent(*dat);
                    break;
                }
                
                MouseHandlerList::reverse_iterator it;
                for (it = m_mouseHandlers.rbegin(); it != m_mouseHandlers.rend(); ++it) {
                    if ((*it)->getInputMask() & MSCROLL_MASK)
                        (*it)->HandleMScrollEvent(*dat);
                }
                break;
            }
                
            default:
                break;
        }
        q.pop();
    }
    
    while (!m_addQueue.empty()) {
        m_mouseHandlers.push_back(m_addQueue.front());
        m_addQueue.pop();
    }
}
