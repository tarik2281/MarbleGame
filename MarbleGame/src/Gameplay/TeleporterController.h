//
//  TeleporterController.h
//  MarbleGame
//
//  Created by Tarik Karaca on 10.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__TeleporterController__
#define __MarbleGame__TeleporterController__

#include <string>

#include "GameLogic.h"
#include "EventManager/EventManager.h"

class TeleporterController : public IObjectController, public IEventListener {
public:
    void Initialize(GameLogic*);
    void Update();
    void Release(GameLogic*);
    
    void HandleEvent(const EventDataPtr&);
    
    std::string m_targetName;
    ObjectID m_targetId;
    float m_time;
    bool m_ported;
	bool m_porting;

	TeleporterController() {
		m_ported = false;
		m_porting = false;
		m_time = 0.0f;
	}
};

#endif /* defined(__MarbleGame__TeleporterController__) */
