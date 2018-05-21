//
//  ButtonController.h
//  MarbleGame
//
//  Created by Tarik Karaca on 04.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__ButtonController__
#define __MarbleGame__ButtonController__

#include "EventManager/EventManager.h"
#include "GameLogic.h"

class ButtonController : public IObjectController, public IEventListener {
    bool m_pushDown;
    bool m_release;
    
    bool m_isDown;
    bool m_moveUp;
public:
    ButtonController() {
		m_pushDown = false;
		m_release = false;

		m_isDown = false;
		m_moveUp = false;

		m_remove = false;
	}
    
    void Initialize(GameLogic*);
    void Release(GameLogic*);
    void Update();
    
    void HandleEvent(const EventDataPtr&);
    void SetMoveUp(bool moveUp) {
        m_moveUp = moveUp;
    }
    
    bool m_remove;
};

#endif /* defined(__MarbleGame__ButtonController__) */
