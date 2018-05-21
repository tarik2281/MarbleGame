//
//  PlayerController.h
//  MarbleGame
//
//  Created by Tarik Karaca on 05.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__PlayerController__
#define __MarbleGame__PlayerController__

#include "Physics/Physics.h"
#include "Scene/SceneNode.h"
#include "EventManager/EventManager.h"
#include "InputManager/InputManager.h"

class Scene;
class GameLogic;

class PlayerController : public IKeyInputHandler, public IEventListener {
public:
    void Initialize(Material, GameLogic*);
    void Update();
    void Release(GameLogic*);
    void ResetPlayer();

	void HandleEvent(const EventDataPtr&);
    
    bool HandleKeyEvent(const KeyInputData&);
    
    PlayerController(Scene* scene) {
		m_pScene = scene;
		m_moveX = 0;
		m_moveZ = 0;
		m_isFalling = false;
		m_respawn = VectorZero;
		m_powerUp = false;
		m_changing = false;
		m_time = 0.0f;
	}
private:
    PhysicsBody* m_pPlayerBody;
    Scene* m_pScene;
    SceneNodePtr m_pPlayerSceneNode;
	GameLogic* m_pLogic;
	Vector3 m_respawn;
    
    int m_moveX;
    int m_moveZ;
    bool m_isFalling;
	bool m_powerUp;
	bool m_changing;
	float m_time;
	Vector3 oldpos;
	Vector3 oldPhyspos;
};

#endif /* defined(__MarbleGame__PlayerController__) */
