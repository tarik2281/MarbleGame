//
//  GameplayScreen.h
//  MarbleGame
//
//  Created by Tarik Karaca on 07.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__GameplayScreen__
#define __MarbleGame__GameplayScreen__

#include "ScreenManager.h"
#include "InputManager/InputManager.h"
#include "EventManager/EventManager.h"
#include "Physics/Physics.h"
//#include "UIScreen.h"
#include "Gameplay/GameLogic.h"
#include "UserInterface/Font.h"
//#include "LabelUI.h"

class PlayerController;
class CameraController;
class ButtonController;
class LookAtCamera;
class Level;
class PhysicsDebugDrawer;
class FontLabel;
class Scene;
class CEffectManager;

class GameplayScreen : public IScreen, public IKeyInputHandler, public IEventListener {
    Scene* m_pScene;
    GameLogic* m_pGameLogic;
    PlayerController* m_pPlayerController;
    CameraController* m_pCameraController;
    LookAtCamera* m_pCamera;
    SceneNodePtr m_pSkyboxNode;
    Level* m_pLevel;
    bool m_paused;
    bool m_remove;
	bool m_normalMapping;
    PhysicsDebugDrawer* m_pDrawer;
	int starsCollected;

	bool powerCollected;
	ID3D11Buffer* spriteBuffer;
	Texture2D* spriteTexture;
	CBaseEffect* m_pSpriteEffect;

	CEffectManager* m_pUIEffectManager;
	FontData m_pFontData;
	ID3D11Buffer* m_pcbFontData;
	FontLabel* label;
    
    void InitializePlayer();
public:
    void TransitionOff() { };
    void TransitionOn() { };
    void Update();
    void Render();
    void Initialize();
    void Release();

	void SetNormalMapping(bool state) {
		m_normalMapping = state;
	}
    
    bool HandleKeyEvent(const KeyInputData&);
    void HandleEvent(const EventDataPtr&);
    
    GameplayScreen() : IScreen() {
		m_paused = false;
		m_remove = false;
		starsCollected = 0;
		powerCollected = false;
	}
    ~GameplayScreen() { Release(); }
};

/*lass TimeScreen : public IScreen, public IEventListener {
    bool m_paused = false;
    float m_currentTime = 0.0f;
    bool m_remove = false;
public:
    void Initialize();
    void Update();
    void Render();
    
    void HandleEvent(const EventDataPtr&);
    
    TimeScreen() { }
    ~TimeScreen();
};*/

/*class GameMenuScreen : public UIScreen {
    bool m_paused = false;
    LabelPtr m_label;
    float m_time = 0.0f;
public:
    void Update();
    
    void Initialize();
    void ClickHandler();
    
    void HandleEvent(const EventDataPtr&);
    
    ~GameMenuScreen();
};*/

#endif /* defined(__MarbleGame__GameplayScreen__) */
