//
//  ScreenManager.h
//  MarbleGame
//
//  Created by Tarik Karaca on 07.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__ScreenManager__
#define __MarbleGame__ScreenManager__

// Transitions between screen switches
// modals for dialog boxes, etc.
// user interface controls

#include <list>
#include <memory>

#include "EventManager/EventManager.h"
#include "Maths/MatrixMath.h"

struct CSize {
    int width, height;
};

class IScreen;
class ScreenManager;

typedef std::shared_ptr<IScreen> ScreenPtr;

class IScreen : public std::enable_shared_from_this<IScreen> {
    friend class ScreenManager;
    
    bool m_inited;
    bool m_isDone; // for properly removing from ScreenManager and other subsystems while iterating
    
protected:
    ScreenManager* m_pScreenManager;
    
public:
    virtual void Render() = 0;
    virtual void Update() = 0;
    virtual void Initialize() = 0;
    virtual void Release() { }
    virtual void OnAdded();
    virtual void OnRemoved() { }
    virtual void ExitScreen() { m_isDone = true; }
    
    void SetScreenManager(ScreenManager* manager) { m_pScreenManager = manager; }
    
	IScreen() {
		m_inited = false;
		m_isDone = false;
	}
    virtual ~IScreen() { }
};


class ScreenManager : public IEventListener {
private:
    typedef std::list<ScreenPtr> ScreenList;
    
    ScreenList m_screens;
    Matrix4 m_orthoMatrix;
    CSize m_screenSize;
    
public:
    void Initialize(int screenWidth, int screenHeight);
    void Release();
    void Update();
    void Render();
    
    void AddScreen(const ScreenPtr&);
    void RemoveScreen(const ScreenPtr&);
    void SwitchToScreen(const ScreenPtr&);
    
    const Matrix4& getOrthoMatrix() const {
        return m_orthoMatrix;
    }
    
    CSize GetScreenSize() const {
        return m_screenSize;
    }
    
    void HandleEvent(const EventDataPtr&);
};


inline void IScreen::OnAdded() {
    if (!m_inited) {
        Initialize();
        m_inited = true;
    }
}

#endif /* defined(__MarbleGame__ScreenManager__) */
