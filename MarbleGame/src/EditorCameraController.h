#ifndef _EDITOR_CAMERA_CONTROLLER_H_
#define _EDITOR_CAMERA_CONTROLLER_H_

#include "InputManager/InputManager.h"

class LookAtCamera;

class EditorCameraController : public IMouseInputHandler
{
	bool m_rotateCam;
	bool m_moveCam;
	LookAtCamera* m_camera;
public:
	void Initialize();
	void Release();
	void Update();

	EditorCameraController(LookAtCamera* camera) : m_camera(camera) { m_rotateCam = false; }

	bool HandleMButtonEvent(const MButtonInputData&);
	bool HandleMMotionEvent(const MMotionInputData&);
	bool HandleMScrollEvent(const MScrollInputData&);
};

#endif

