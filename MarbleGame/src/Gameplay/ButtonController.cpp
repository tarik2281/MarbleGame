//
//  ButtonController.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 04.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "ButtonController.h"
#include "Level.h"
#include "EventManager/Events.h"
#include "../main.h"

#define BUTTON_SPEED 0.01f
#define MOVE_SPEED 0.05f

void ButtonController::Initialize(GameLogic* logic) {
    g_pEventManager->AddEventListener(this, CollisionEvent::sk_eventType);
}

void ButtonController::Release(GameLogic* logic) {
    g_pEventManager->RemoveEventListener(this, CollisionEvent::sk_eventType);
}

void ButtonController::Update() {
    ButtonNode* node = static_cast<ButtonNode*>(m_pObject->GetSceneNode().get());
    
    if (m_pushDown) {
        node->y += BUTTON_SPEED;
        
        if (node->y > 0.12926f)
            m_pushDown = false;
    }
    if (m_release) {
        node->y -= BUTTON_SPEED;
        
        if (node->y < 0.0f)
            m_release = false;
    }
    
	if (m_isDown) {
		GameObjectPtr object = m_pGameLogic->GetObject(m_pGameLogic->GetObjectID("Platform4"));
		GameObjectPtr player = m_pGameLogic->GetObject(m_pGameLogic->GetObjectID("Player"));
		if (object) {
			object->MoveObjectBy(VectorUp * (m_moveUp ? MOVE_SPEED : -MOVE_SPEED));
			//player->MoveObjectBy(VectorUp * (m_moveUp ? MOVE_SPEED : -MOVE_SPEED));
		}
    }
}

void ButtonController::HandleEvent(const EventDataPtr &data) {
    if (data->getEventType() == CollisionEvent::sk_eventType) {
        CollisionEvent* event = static_cast<CollisionEvent*>(data.get());
        if (IS_COLL_BODY(event, m_pObject->GetPhysicsBody())) {
            if (event->m_added) {
                m_pushDown = true;
                m_release = false;
                m_isDown = true;
                
                if (m_remove)
                    m_pGameLogic->RemoveObject(m_pGameLogic->GetObjectID("Platform4"));
            } else {
                m_pushDown = false;
                m_release = true;
                m_isDown = false;
            }
        }
    }
}
