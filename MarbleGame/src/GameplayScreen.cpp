//
//  GameplayScreen.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 07.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "GameplayScreen.h"
#include "Gameplay/PlayerController.h"
#include "Gameplay/CameraController.h"
#include "Scene/Camera.h"
#include "Scene/Skybox.h"
#include "Resources/Resource.h"
#include "Resources/ResourceManager.h"
#include "Gameplay/Level.h"
//#include "MenuScreen.h"
#include "EventManager/Events.h"
//#include "PhysicsDebugDrawer.h"
#include "Resources/MaterialList.h"
#include "UserInterface/Font.h"
#include "Scene/Terrain.h"
#include "Scene/InstancingNode.h"
#include "Scene/SceneNode.h"
#include "main.h"
//#include "LabelUI.h"

float currentTime = 0.0f;

void GameplayScreen::Initialize() {
    m_pScene = DBG_NEW Scene;
    m_pScene->Initialize(Vector3(-1.0f, 0.8f, -0.3f).Normalize());
    
    CSize screenSize = m_pScreenManager->GetScreenSize();
    
    m_pCamera = DBG_NEW LookAtCamera;
    m_pCamera->setProjection(MatrixMakePerspective(65, float(screenSize.width) / float(screenSize.height), 0.1f, 300.0f));
    m_pCamera->setZoom(10.0f);
	//m_pCamera->setRotationX(-45.0f);
	//m_pCamera->setRotationY(45.0f);
	m_pCamera->setRotationX(0.0f);
	m_pCamera->setRotationY(0.0f);
	m_pScene->SetCamera(m_pCamera);

	//bitmapShader = g_pResourceManager->LoadContent<Shader>("..\\BitmapShader.cso");
	//bitmapShader->Initialize("FontTechnique", "FontPass", VertexInputPosTex);
	Matrix4 ortho = MatrixMakeOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
	//bitmapShader->SetVariable("projectionMatrix", &ortho);

	StarInstances* insts = new StarInstances();
	insts->m_model = g_pResourceManager->QueueContent<Model>("Star.obj");
	m_pScene->AddInstancingNode(MakeInstancingNodePtr(insts));

    m_pGameLogic = DBG_NEW GameLogic(m_pScene);
    m_pLevel = DBG_NEW Level;
    m_pLevel->LoadLevel((ResourceManager::GetResourcesPath() + "Level.txt").c_str());
    m_pGameLogic->InitializeFromLevel(m_pLevel);

	insts->InitData();
    
    m_pPlayerController = DBG_NEW PlayerController(m_pScene);
    Material playerMaterial = g_pMaterials->GetVisualMaterial("Marble");
    m_pPlayerController->Initialize(playerMaterial, m_pGameLogic);
    
    m_pCameraController = DBG_NEW CameraController(m_pCamera);
    m_pCameraController->Initialize();
    
    SkyboxNode* skybox = DBG_NEW SkyboxNode;
	m_pSkyboxNode = MakeNodePtr(skybox);
	skybox->init();
	m_pScene->AddNode(m_pSkyboxNode);

    
    g_pInputManager->AddKeyInputHandler(HotKey(27, 0), this);
    g_pInputManager->AddKeyInputHandler(HotKey('p', 0), this);
	g_pInputManager->AddKeyInputHandler(HotKey('M', 0), this);
	g_pInputManager->AddKeyInputHandler(HotKey('C', 0), this);
    g_pEventManager->AddEventListener(this, GamePausedEvent::sk_eventType);
    g_pEventManager->AddEventListener(this, GameExitEvent::sk_eventType);
    g_pEventManager->AddEventListener(this, ContextResizeEvent::sk_eventType);
	g_pEventManager->AddEventListener(this, StarCollectEvent::sk_eventType);
	g_pEventManager->AddEventListener(this, PowerUpCollectEvent::sk_eventType);
	g_pEventManager->AddEventListener(this, PlayerLostEvent::sk_eventType);
    //m_pDrawer = DBG_NEW PhysicsDebugDrawer;
    //m_pGameLogic->SetDebugDrawer(m_pDrawer);


	TEMP_DEVICE(pDevice);

	GlyphVertex v[6];
	v[0].position = Vector3(50.0f, 75.0f, 0.0f);
	v[0].texCoord = Vector2(0.0f, 1.0f);
	v[1].position = Vector3(50.0f, 139.0f, 0.0f);
	v[1].texCoord = Vector2(0.0f, 0.0f);
	v[2].position = Vector3(114.0f, 75.0f, 0.0f);
	v[2].texCoord = Vector2(1.0f, 1.0f);
	v[3].position = Vector3(50.0f, 139.0f, 0.0f);
	v[3].texCoord = Vector2(0.0f, 0.0f);
	v[4].position = Vector3(114.0f, 139.0f, 0.0f);
	v[4].texCoord = Vector2(1.0f, 0.0f);
	v[5].position = Vector3(114.0f, 75.0f, 0.0f);
	v[5].texCoord = Vector2(1.0f, 1.0f);

	MakeBuffer(pDevice, spriteBuffer, false, D3D11_BIND_VERTEX_BUFFER, v, 6 * sizeof(GlyphVertex));

	spriteTexture = g_pResourceManager->QueueContent<Texture2D>("Icon.png");

	m_pUIEffectManager = DBG_NEW CEffectManager();
	m_pUIEffectManager->Initialize();
	m_pUIEffectManager->SetIsShading(true);

	m_pSpriteEffect = m_pUIEffectManager->LoadEffect("BitmapShader", TexCoord2D, false, false);
	
	//m_pFontData->mProjection = MatrixMakeOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
	m_pFontData.fScreenWidth = (float)SCREEN_WIDTH;
	m_pFontData.fScreenHeight = (float)SCREEN_HEIGHT;
	m_pFontData.fDestAngle = 0.0f;
	MakeDynamicBuffer(pDevice, m_pcbFontData, D3D11_BIND_CONSTANT_BUFFER, sizeof(FontData));
	g_pFont = g_pResourceManager->QueueFont("Arial", m_pUIEffectManager);

    g_pResourceManager->LoadFromQueue();

	label = g_pFont->MakeNewLabel(L"Stars: 0/" + std::to_wstring((long long)m_pLevel->StarsCount()), Vector2(50.0f, 50.0f));
}

void GameplayScreen::Release() {
	spriteBuffer->Release();
    
    g_pEventManager->RemoveEventListener(this, GamePausedEvent::sk_eventType);
    g_pEventManager->RemoveEventListener(this, GameExitEvent::sk_eventType);
    g_pEventManager->RemoveEventListener(this, ContextResizeEvent::sk_eventType);
	g_pEventManager->RemoveEventListener(this, StarCollectEvent::sk_eventType);
	g_pEventManager->RemoveEventListener(this, PowerUpCollectEvent::sk_eventType);
	g_pEventManager->RemoveEventListener(this, PlayerLostEvent::sk_eventType);
    g_pInputManager->RemoveKeyInputHandler(HotKey(27, 0));
    g_pInputManager->RemoveKeyInputHandler(HotKey('p', 0));
	g_pInputManager->RemoveKeyInputHandler(HotKey('M', 0));
	g_pInputManager->RemoveKeyInputHandler(HotKey('C', 0));
   
	m_pUIEffectManager->Release();
	SAFE_RELEASE(m_pUIEffectManager);

    m_pPlayerController->Release(m_pGameLogic);
    m_pCameraController->Release();
    m_pGameLogic->Release();
    delete m_pGameLogic;
    m_pLevel->Release();

	DX_SAFE_RELEASE(m_pcbFontData);

    
    //delete m_pDrawer;
    
    delete m_pScene;
    delete m_pCamera;
    delete m_pPlayerController;
    delete m_pCameraController;
    delete m_pLevel;
    
    m_pSkyboxNode = nullptr;
}

void GameplayScreen::Update() {
    if (!m_paused) {
		m_pScene->UpdateAnimations();

		if (powerCollected)
			m_pGameLogic->Update(1.0f / (g_fMaxFPS * 2.0f));
		else
			m_pGameLogic->Update();
		

        m_pPlayerController->Update();
        m_pCameraController->Update();
    }
}

void GameplayScreen::Render() {
	TEMP_CONTEXT(pContext);
	
	m_pScene->UpdateData();
    m_pScene->Begin();
	g_pGraphics->SetBackBuffer();
	g_pGraphics->SetDefaultRSState();
	m_pScene->Draw(nullptr, nullptr);

	m_pUIEffectManager->Begin();

	m_pFontData.fScreenWidth = SCREEN_WIDTH;
	m_pFontData.fScreenHeight = SCREEN_HEIGHT;
	m_pFontData.fDestAngle = (currentTime / 10.0f) * 360.0f;
	UpdateBufferStruct(pContext, m_pcbFontData, m_pFontData);
	pContext->VSSetConstantBuffers(0, 1, &m_pcbFontData);
	pContext->PSSetConstantBuffers(0, 1, &m_pcbFontData);

	g_pGraphics->DisableDepth();
	g_pGraphics->EnableBlending();
	label->Render();

	if (powerCollected) {
		if (currentTime >= 10.0f) {
			g_pEventManager->QueueEvent(EventDataPtr(new PowerUpCollectEvent()));
		}

		UINT stride = sizeof(GlyphVertex);
		UINT offset = 0;

		pContext->IASetVertexBuffers(0, 1, &spriteBuffer, &stride, &offset);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		currentTime += 1.0f / 60.0f;

		m_pUIEffectManager->ApplyEffect(m_pSpriteEffect);
		pContext->PSSetShaderResources(0, 1, &spriteTexture->m_pView);
		pContext->Draw(6, 0);
	}

	g_pGraphics->DisableBlending();
	g_pGraphics->SetDefaultDSState();

	/*if (powerCollected) {
		if (currentTime >= 10.0f) {
			g_pEventManager->QueueEvent(EventDataPtr(new PowerUpCollectEvent()));
		}
		TEMP_CONTEXT(pContext);

		UINT stride = sizeof(Vector3)+sizeof(Vector2);
		UINT offset = 0;
		pContext->IASetVertexBuffers(0, 1, &spriteBuffer, &stride, &offset);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		bitmapShader->SetVariable("someTexture", spriteTexture->m_pView);
		float destAngle = (currentTime / 10.0f) * 360.0f;
		bitmapShader->SetVariable("destAngle", &destAngle);
		currentTime += 1.0f / 60.0f;
		bitmapShader->Apply();
		pContext->Draw(6, 0);
	}*/
}

bool GameplayScreen::HandleKeyEvent(const KeyInputData &data) {
    if (data.m_state) {
        if (data.m_keyCode == 27) {
            //PauseScreen* scr = DBG_NEW PauseScreen;
            //m_pScreenManager->AddScreen(ScreenPtr(scr));
        } else if (data.m_keyCode == 'p') {
            //physicsDebugDrawing = !physicsDebugDrawing;
		}
		else if (data.m_keyCode == 'M') {
			m_pScene->NextCam();
		}
		else if (data.m_keyCode == 'C') {
			m_pScene->ToggleVisualCascades();
		}
    }
    return false;
}

void GameplayScreen::HandleEvent(const EventDataPtr &event) {
    EventType type = event->getEventType();
    if (type == GameExitEvent::sk_eventType) {
        ExitScreen();
    } else if (type == GamePausedEvent::sk_eventType) {
        GamePausedEvent* gpEvent = static_cast<GamePausedEvent*>(event.get());
        m_paused = gpEvent->m_paused;
    } else if (type == ContextResizeEvent::sk_eventType) {
        ContextResizeEvent* crEvent = static_cast<ContextResizeEvent*>(event.get());
        m_pCamera->setProjection(MatrixMakePerspective(65, float(crEvent->m_screenWidth) / float(crEvent->m_screenHeight), 1.0f, 1000.0f));
	}
	else if (type == StarCollectEvent::sk_eventType) {
		starsCollected++;
		label->SetText(L"Stars: " + std::to_wstring((long long)starsCollected) + L"/" + std::to_wstring((long long)m_pLevel->StarsCount()), Vector2(50.0f, 50.0f));
	}
	else if (type == PowerUpCollectEvent::sk_eventType) {
		powerCollected = !powerCollected;
		currentTime = 0;
	}
	else if (type == PlayerLostEvent::sk_eventType) {
		starsCollected = 0;
		ObjectIterator it = m_pLevel->ObjectsBegin();
		while (it != m_pLevel->ObjectsEnd()) {
			if ((*it)->getFlag() == STAR_FLAG) {
				m_pGameLogic->AddObject(it->get());
			}
			++it;
		}
		label->SetText(L"Stars: " + std::to_wstring((long long)starsCollected) + L"/" + std::to_wstring((long long)m_pLevel->StarsCount()), Vector2(50.0f, 50.0f));
	}
}


/*void TimeScreen::Initialize() {
    g_pEventManager->AddEventListener(this, GamePausedEvent::sk_eventType);
    g_pEventManager->AddEventListener(this, GameExitEvent::sk_eventType);
}

TimeScreen::~TimeScreen() {
    g_pEventManager->RemoveEventListener(this, GamePausedEvent::sk_eventType);
    g_pEventManager->RemoveEventListener(this, GameExitEvent::sk_eventType);
}

void TimeScreen::Update() {
    if (!m_paused)
        m_currentTime += 1.0f / 60.0f;
}

void TimeScreen::Render() {
    Shader::Unbind();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(m_pScreenManager->getOrthoMatrix().m);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    g_pFont->DrawLine(L"Elapsed Time: " + std::to_wstring(m_currentTime), Vector2(0.0f, 0.0f));
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void TimeScreen::HandleEvent(const EventDataPtr &event) {
    EventType type = event->getEventType();
    if (type == GameExitEvent::sk_eventType) {
        ExitScreen();
    } else if (type == GamePausedEvent::sk_eventType) {
        GamePausedEvent* gpEvent = static_cast<GamePausedEvent*>(event.get());
        m_paused = gpEvent->m_paused;
    }
}*/


/*void GameMenuScreen::Initialize() {
    UIScreen::Initialize();
    
    CSize screenSize = m_pScreenManager->GetScreenSize();
    
    ButtonUI* button = DBG_NEW ButtonUI;
    button->SetText(L"Pause");
    button->SetOrigin(1.0f, 1.0f);
    button->m_delegate = ButtonClickDelegate::MakeDelegate(GameMenuScreen, ClickHandler, this);
    AddControl(ControlPtr(button));
    
    m_label = LabelPtr(DBG_NEW LabelUI);
    m_label->SetText(L"Elapsed Time:");
    m_label->SetOrigin(0.0f, 0.0f);
    AddControl(m_label);
    
    g_pEventManager->AddEventListener(this, GameExitEvent::sk_eventType);
    g_pEventManager->AddEventListener(this, GamePausedEvent::sk_eventType);
}

void GameMenuScreen::Update() {
    UIScreen::Update();
    
    if (!m_paused) {
        m_time += 1.0f / 60.0f;
        m_label->SetText(L"Elapsed Time: " + std::to_wstring(m_time));
    }
}

void GameMenuScreen::ClickHandler() {
    PauseScreen* scr = DBG_NEW PauseScreen;
    m_pScreenManager->AddScreen(ScreenPtr(scr));
}

void GameMenuScreen::HandleEvent(const EventDataPtr &event) {
    UIScreen::HandleEvent(event);
    
    EventType type = event->getEventType();
    if (type == GameExitEvent::sk_eventType) {
        ExitScreen();
    } else if (type == GamePausedEvent::sk_eventType) {
        GamePausedEvent* gpEvent = static_cast<GamePausedEvent*>(event.get());
        m_paused = gpEvent->m_paused;
    }
}

GameMenuScreen::~GameMenuScreen() {
    g_pEventManager->RemoveEventListener(this, GameExitEvent::sk_eventType);
    g_pEventManager->RemoveEventListener(this, GamePausedEvent::sk_eventType);
}*/
