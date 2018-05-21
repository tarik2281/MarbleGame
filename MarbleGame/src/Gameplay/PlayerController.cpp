//
//  PlayerController.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 05.01.14.
//  Copyright (c) 2014 Tarik Karaca. A5ll rights reserved.
//

#include "PlayerController.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Resources/Model.h"
#include "GameLogic.h"
#include "Resources/ResourceManager.h"
#include "Physics/NodeMotionState.h"
#include "EventManager/Events.h"
#include "Level.h"
#include "../main.h"

#define M_PI 3.14159265f
#define PORT_TIME (15.0f/60.0f)

void PlayerController::Initialize(Material mat, GameLogic* gameLogic) {
	mat.opacity = 1.0f;
    Model* model = g_pResourceManager->QueueContent<Model>("Marble4.obj");
    m_pPlayerSceneNode = MakeNodePtr(DBG_NEW ModelNode(model, mat));
    
    PhysicsShape* playerShape = g_pPhysics->MakeSphereShape(0.99f);
	m_respawn = gameLogic->m_respawnPosition;
    NodeMotionState* state = DBG_NEW NodeMotionState(m_pPlayerSceneNode.get(), m_respawn);
	m_pPlayerBody = g_pPhysics->MakeBody(playerShape, state, (((2.0f * 2.0f * 2.0f) * 1000.0f) * 0.8f) / 1000.0f);
    m_pPlayerBody->setRestitution(0.65f);
    m_pPlayerBody->setFriction(0.5f);
    m_pPlayerBody->setRollingFriction(0.3f);
    m_pPlayerBody->setActivationState(DISABLE_DEACTIVATION);
    gameLogic->AddObject("Player", m_pPlayerBody, m_pPlayerSceneNode);
    
    g_pInputManager->AddKeyInputHandler(HotKey('A', 0), this);
    g_pInputManager->AddKeyInputHandler(HotKey('S', 0), this);
    g_pInputManager->AddKeyInputHandler(HotKey('W', 0), this);
    g_pInputManager->AddKeyInputHandler(HotKey('D', 0), this);
    g_pInputManager->AddKeyInputHandler(HotKey(' ', 0), this);

	g_pEventManager->AddEventListener(this, PowerUpCollectEvent::sk_eventType);

	m_pLogic = gameLogic;
}

void PlayerController::Release(GameLogic* logic) {
    logic->RemoveObject("Player");
    
	g_pEventManager->RemoveEventListener(this, PowerUpCollectEvent::sk_eventType);

    g_pInputManager->RemoveKeyInputHandler(HotKey('A', 0));
    g_pInputManager->RemoveKeyInputHandler(HotKey('S', 0));
    g_pInputManager->RemoveKeyInputHandler(HotKey('W', 0));
    g_pInputManager->RemoveKeyInputHandler(HotKey('D', 0));
    g_pInputManager->RemoveKeyInputHandler(HotKey(' ', 0));
    
    m_pScene->RemoveNode(m_pPlayerSceneNode);
    m_pPlayerSceneNode = nullptr;
}

void PlayerController::Update() {
    float fYVel = m_pPlayerBody->getLinearVelocity().y();
    m_isFalling = (fYVel < -0.5f || fYVel > 0.5f);
    
	if (m_moveX || m_moveZ) {
		LookAtCamera* cam = (LookAtCamera*)m_pScene->GetCamera();
		float cosRes = cosf(-cam->getRotationY() * M_PI / 180.0f);
		float sinRes = sinf(-cam->getRotationY() * M_PI / 180.0f);
		float mass = 1.0f / m_pPlayerBody->getInvMass();
		float speed = (m_pLogic->m_isSmall ? 20.0f : 20.0f);
		float xForce = (speed * m_moveX * mass) * cosRes - (speed * m_moveZ * mass) * sinRes;
		float zForce = (speed * m_moveX * mass) * sinRes + (speed * m_moveZ * mass) * cosRes;
		m_pPlayerBody->applyCentralForce(btVector3(xForce, 0.0f, zForce));
	}
    
    if (m_pPlayerSceneNode->GetOrigin().y < -100.0f) {
        EventDataPtr event(DBG_NEW PlayerLostEvent);
        g_pEventManager->QueueEvent(event);
        ResetPlayer();
    }

	if (m_changing) {
		m_time += 1.0f / 60.0f;

		if (m_pLogic->m_isSmall) {
			float step = 0.5f / (10.0f / 60.0f) / 60.0f;
			Vector3 orig = m_pPlayerSceneNode->GetOrigin();
			float offset = m_pPlayerBody->getWorldTransform().getOrigin().y() - oldPhyspos.y;
			oldPhyspos = m_pPlayerBody->getWorldTransform().getOrigin();
			orig.y = oldpos.y - step + offset;
			oldpos = orig;
			m_pPlayerSceneNode->SetOrigin(orig);
		}
		else {
			float step = 0.5f / (10.0f / 60.0f) / 60.0f;
			Vector3 orig = m_pPlayerSceneNode->GetOrigin();
			float offset = m_pPlayerBody->getWorldTransform().getOrigin().y() - oldPhyspos.y;
			oldPhyspos = m_pPlayerBody->getWorldTransform().getOrigin();
			orig.y = oldpos.y + step +offset;
			oldpos = orig;
			m_pPlayerSceneNode->SetOrigin(orig);
		}

		if (m_time >= 10.0f / 60.0f)
			m_changing = false;
	}

	m_pScene->GetCamera()->setPosition(m_pPlayerSceneNode->GetOrigin());
}

void PlayerController::HandleEvent(const EventDataPtr& data) {
	if (data->getEventType() == PowerUpCollectEvent::sk_eventType) {
		m_powerUp = !m_powerUp;
	}
}

bool PlayerController::HandleKeyEvent(const KeyInputData &data) {
    switch (data.m_keyCode) {
        case 'W':
            m_moveZ = (data.m_state) ? -1 : 0;
            break;
        case 'D':
            m_moveX = (data.m_state) ? 1 : 0;
            break;
        case 'S':
            m_moveZ = (data.m_state) ? 1 : 0;
            break;
        case 'A':
            m_moveX = (data.m_state) ? -1 : 0;
            break;
        case ' ': {
					  if (data.m_state) {
						  m_pLogic->m_isSmall = !m_pLogic->m_isSmall;
						  m_changing = true;
						  m_time = 0.0f;
						  float endScale = (m_pLogic->m_isSmall ? 0.5f : 1.0f);
						  ScaleNodeAnimation* anim = DBG_NEW ScaleNodeAnimation(m_pPlayerSceneNode, 10.0f / 60.0f, m_pPlayerSceneNode->GetScale().x, endScale);
						  m_pScene->RunAnimation(NodeAnimationPtr(anim));
						  Vector3 velocity = m_pPlayerBody->getLinearVelocity();
						  Vector3 rotVel = m_pPlayerBody->getAngularVelocity();
						  btTransform trans = m_pPlayerBody->getWorldTransform();
						  m_pLogic->RemoveObject(m_pLogic->GetObjectID("Player"));
						  PhysicsShape* playerShape = g_pPhysics->MakeSphereShape(endScale - 0.01f);
						  NodeMotionState* state = DBG_NEW NodeMotionState(m_pPlayerSceneNode.get(), m_pPlayerSceneNode->GetOrigin());
						  oldpos = m_pPlayerSceneNode->GetOrigin();
						  float vol = (m_pLogic->m_isSmall ? 1.0f : 8.0f);
						  m_pPlayerBody = g_pPhysics->MakeBody(playerShape, state, ((vol * 1000.0f) * 0.8f) / 1000.0f);
						  m_pPlayerBody->setRestitution(0.65f);
						  m_pPlayerBody->setFriction(0.5f);
						  m_pPlayerBody->setRollingFriction((m_pLogic->m_isSmall ? 0.3f / 8.0f : 0.3f));
						  m_pPlayerBody->setActivationState(DISABLE_DEACTIVATION);
						  if (m_pLogic->m_isSmall) {
							  trans.setOrigin(Vector3(oldpos.x, oldpos.y - 0.5f, oldpos.z));
							  oldPhyspos = Vector3(oldpos.x, oldpos.y - 0.5f, oldpos.z);
						  }
						  else {
							  trans.setOrigin(Vector3(oldpos.x, oldpos.y + 0.5f, oldpos.z));
							  oldPhyspos = Vector3(oldpos.x, oldpos.y + 0.5f, oldpos.z);
						  }
						  m_pPlayerBody->setWorldTransform(trans);
						  m_pPlayerBody->setLinearVelocity(velocity);
						  m_pPlayerBody->setAngularVelocity(rotVel);
						  m_pLogic->AddObject("Player", m_pPlayerBody, m_pPlayerSceneNode);
					  }
            /*if (!m_isFalling && data.m_state) {
                float mass = 1.0f / m_pPlayerBody->getInvMass();
                float impulse = mass * 20.0f;
				
				m_pPlayerBody->applyCentralImpulse(btVector3(0.0f, impulse * (m_powerUp ? 2.0f : 1.0f), 0.0f));
            }*/
            break;
        }
        default:
            break;
    }
    
    return false;
}

void PlayerController::ResetPlayer() {
    btTransform transform;
	transform.setOrigin(m_respawn);
    transform.setRotation(btQuaternion(0, 0, 0, 1));
    m_pPlayerBody->setWorldTransform(transform);
    m_pPlayerBody->setAngularVelocity(btVector3(0, 0, 0));
    m_pPlayerBody->setLinearVelocity(btVector3(0, 0, 0));

	float endScale = (m_pLogic->m_isSmall ? 0.5f : 1.0f);
	ScaleNodeAnimation* anim = DBG_NEW ScaleNodeAnimation(m_pPlayerSceneNode, 15.0f / 60.0f, 0.0f, endScale);
	m_pScene->RunAnimation(NodeAnimationPtr(anim));
}
