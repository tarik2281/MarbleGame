//
//  CameraController.h
//  MarbleGame
//
//  Created by Tarik Karaca on 07.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__CameraController__
#define __MarbleGame__CameraController__

#include "InputManager/InputManager.h"

class LookAtCamera;

class CameraController : public IMouseInputHandler {
    bool m_rotateCam;
    LookAtCamera* m_camera;
public:
    void Initialize();
    void Release();
    void Update();
    
    CameraController(LookAtCamera* camera) : m_camera(camera) { m_rotateCam = false; }
    
    bool HandleMButtonEvent(const MButtonInputData&);
    bool HandleMMotionEvent(const MMotionInputData&);
    bool HandleMScrollEvent(const MScrollInputData&);
};

#endif /* defined(__MarbleGame__CameraController__) */
