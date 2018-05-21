//
//  CameraController.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 07.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "CameraController.h"
#include "Scene/Camera.h"
#include "../Main.h"

void CameraController::Initialize() {
    g_pInputManager->AddMouseInputHandler(this);
    
    m_inputMask |= MSCROLL_MASK;
}

void CameraController::Release() {
    g_pInputManager->RemoveMouseInputHandler(this);
}

void CameraController::Update() {
    
}

bool CameraController::HandleMButtonEvent(const MButtonInputData &data) {
    if (data.m_button == 1) {
        if (m_rotateCam) {
            if (!data.m_state) {
                g_pInputManager->ReleaseSingleReceiver();
            }
        } else {
            if (data.m_state) {
                g_pInputManager->SetSingleReceiver(this);
            }
        }
        m_rotateCam = data.m_state;
        return true;
    }
    return false;
}

bool CameraController::HandleMMotionEvent(const MMotionInputData &data) {
    if (m_rotateCam) {
        m_camera->addRotationX(data.m_deltaY * 0.5f);
        m_camera->addRotationY(data.m_deltaX * 0.5f);
        return true;
    }
    return false;
}

bool CameraController::HandleMScrollEvent(const MScrollInputData &data) {
    m_camera->addZoom(-data.m_deltaY);
    if (m_camera->getZoom() < 1.0f)
        m_camera->setZoom(1.0f);
    return true;
}