//
//  InputManager.h
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__InputManager__
#define __MarbleGame__InputManager__

#include <map>
#include <queue>
#include <list>

#include "InputData.h"

#define MBUTTON_MASK 0x1
#define MMOTION_MASK 0x2
#define MSCROLL_MASK 0x4

struct HotKey {
    unsigned char key;
    unsigned int modifiers;
    
    bool operator<(const HotKey& h) const {
        return (key < h.key) || ((key == h.key) && (modifiers < h.modifiers));
    }
    
    bool operator==(const HotKey& h) const {
        return (key == h.key && modifiers == h.modifiers);
    }
    
    HotKey() { }
    HotKey(unsigned char key, unsigned int modifiers) : key(key), modifiers(modifiers) { }
    
    static const HotKey k_Wildcard;
};


class IKeyInputHandler {
public:
    virtual bool HandleKeyEvent(const KeyInputData&) = 0;
};

class IMouseInputHandler {
public:
    virtual bool HandleMButtonEvent(const MButtonInputData&) { return false; }
    virtual bool HandleMMotionEvent(const MMotionInputData&) { return false; }
    virtual bool HandleMScrollEvent(const MScrollInputData&) { return false; }
    const unsigned char getInputMask() const { return m_inputMask; }

	IMouseInputHandler() {
		m_inputMask = MBUTTON_MASK | MMOTION_MASK;
	}
protected:
	unsigned char m_inputMask;
};


class InputManager {
public:
    void AddKeyInputHandler(HotKey, IKeyInputHandler*);
    void RemoveKeyInputHandler(HotKey);
    
    void AddMouseInputHandler(IMouseInputHandler*);
    void RemoveMouseInputHandler(IMouseInputHandler*);
    
    void SetSingleReceiver(IMouseInputHandler*); // mouse input wildcard
    void ReleaseSingleReceiver();
    
    void PushInputEvent(const IInputDataPtr&);
    void PollInputEvents();

	InputManager() {
		m_activeQueue = 0;
		m_processQueue = 0;
		m_keyWildcardInList = false;
		m_singleReceiver = 0;
	}
private:
    typedef std::pair<HotKey, IKeyInputHandler*> HotKeyPair;
    typedef std::map<HotKey, IKeyInputHandler*> HotKeyMap;
    typedef std::list<IMouseInputHandler*> MouseHandlerList;
	typedef std::queue<IMouseInputHandler*> MouseHandlerQueue;
    typedef std::queue<IInputDataPtr> InputEventQueue;
    
    bool m_keyWildcardInList;    
    int m_activeQueue;
    int m_processQueue;
	IMouseInputHandler* m_singleReceiver;
    
	HotKeyMap m_hotKeys;
	MouseHandlerList m_mouseHandlers;
    InputEventQueue m_queues[2];
    MouseHandlerQueue m_addQueue;
    MouseHandlerQueue m_removeQueue;
};


inline void InputManager::AddMouseInputHandler(IMouseInputHandler* handler) {
    if (!handler)
        return;
    
    m_addQueue.push(handler);
}

inline void InputManager::RemoveMouseInputHandler(IMouseInputHandler* handler) {
    if (!handler)
        return;
    
    m_removeQueue.push(handler);
}

inline void InputManager::SetSingleReceiver(IMouseInputHandler* handler) {
    m_singleReceiver = handler;
}

inline void InputManager::ReleaseSingleReceiver() {
    m_singleReceiver = 0;
}

inline void InputManager::PushInputEvent(const IInputDataPtr& data) {
    m_queues[m_activeQueue].push(data);
}

#endif /* defined(__MarbleGame__InputManager__) */
