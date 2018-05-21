//
//  TeleporterController.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 10.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "TeleporterController.h"
#include "Level.h"
#include "EventManager/Events.h"
#include "../main.h"

#define PORT_TIME (10.0f/60.0f)

void TeleporterController::Initialize(GameLogic *logic) {
    g_pEventManager->AddEventListener(this, CollisionEvent::sk_eventType);
    
	GateNode* node = static_cast<GateNode*>(m_pObject->GetSceneNode().get());
	node->texPos = Vector2(0.0f, 0.0f);

    m_targetId = logic->GetObjectID(m_targetName);
}

void TeleporterController::Update() {
	GateNode* node = static_cast<GateNode*>(m_pObject->GetSceneNode().get());
	node->texPos += Vector2(0.0075f, 0.0075f);

	if (node->texPos.x >= 1.0f)
		node->texPos.x = 0.0f;
	if (node->texPos.y >= 1.0f)
		node->texPos.y = 0.0f;

	if (m_porting) {
		m_time += 1.0f / 60.0f;
		GameObjectPtr player = m_pGameLogic->GetObject(m_pGameLogic->GetObjectID("Player"));
		
		if (m_time >= PORT_TIME) {
			m_porting = false;

			GameObjectPtr targetObject = m_pGameLogic->GetObject(m_targetId);

			Vector3 position = targetObject->GetAbsolutePosition();
			player->MoveObjectTo(position);

			TeleporterController* targetControl = static_cast<TeleporterController*>(targetObject->GetController().get());
			targetControl->m_ported = true;
			float scale = (m_pGameLogic->m_isSmall ? 0.5f : 1.0f);
			m_pGameLogic->m_pScene->RunAnimation(NodeAnimationPtr(new ScaleNodeAnimation(player->GetSceneNode(), PORT_TIME, 0.0f, scale)));
		}
	}
}

void TeleporterController::Release(GameLogic *logic) {
    g_pEventManager->RemoveEventListener(this, CollisionEvent::sk_eventType);
}

void TeleporterController::HandleEvent(const EventDataPtr &data) {
    if (data->getEventType() == CollisionEvent::sk_eventType) {
        CollisionEvent* event = static_cast<CollisionEvent*>(data.get());
        if (IS_COLL_BODY(event, m_pObject->GetPhysicsBody())) {
            if (event->m_added && !m_ported) {
                GameObjectPtr targetObject = m_pGameLogic->GetObject(m_targetId);

                if (targetObject) {
					GameObjectPtr player = m_pGameLogic->GetObject(m_pGameLogic->GetObjectID("Player"));
					float scale = (m_pGameLogic->m_isSmall ? 0.5f : 1.0f);
					m_pGameLogic->m_pScene->RunAnimation(NodeAnimationPtr(new ScaleNodeAnimation(player->GetSceneNode(), PORT_TIME, scale, 0.0f)));
					m_porting = true;
					m_time = 0.0f;
                }
            }
            
			if (!event->m_added) {
				m_ported = false;
			}
        }
    }
}