#include "EditorCameraController.h"

#include "main.h"

#include "Scene/Camera.h"

void EditorCameraController::Initialize() {
	g_pInputManager->AddMouseInputHandler(this);

	m_rotateCam = false;
	m_moveCam = false;

	m_inputMask |= MSCROLL_MASK;
}

void EditorCameraController::Release() {
	g_pInputManager->RemoveMouseInputHandler(this);
}

void EditorCameraController::Update() {

}

bool EditorCameraController::HandleMButtonEvent(const MButtonInputData &data) {
	if (data.m_button == MButtonRight) {
		if (data.m_modifierFlags & MODIFIER_SHIFT) {
			m_rotateCam = data.m_state;
			m_moveCam = false;
		}
		else {
			m_rotateCam = false;
			m_moveCam = data.m_state;
		}

		if (data.m_state)
			g_pInputManager->SetSingleReceiver(this);
		else
			g_pInputManager->ReleaseSingleReceiver();
		return true;
	}
	return false;
}

bool EditorCameraController::HandleMMotionEvent(const MMotionInputData &data) {
	if (m_rotateCam) {
		m_camera->addRotationX(data.m_deltaY * 0.5f);
		m_camera->addRotationY(data.m_deltaX * 0.5f);
		return true;
	}
	if (m_moveCam) {
#define M_PI 3.14159265f
#define CAM_MOVING_SPEED 0.005f
		float cosRes = cosf(-m_camera->getRotationY() * M_PI / 180.0f);
		float sinRes = sinf(-m_camera->getRotationY() * M_PI / 180.0f);
		float scale = CAM_MOVING_SPEED * m_camera->getZoom() / 2.5f;
		Vector3 deltaPos((scale * -data.m_deltaX) * cosRes - (scale * -data.m_deltaY) * sinRes, 0.0f,
			(scale * -data.m_deltaX) * sinRes + (scale * -data.m_deltaY) * cosRes);
		m_camera->addPosition(deltaPos);

		return true;
	}
	return false;
}

bool EditorCameraController::HandleMScrollEvent(const MScrollInputData &data) {
	Vector3 deltaPos(0.0f, -data.m_deltaY, 0.0f);
	m_camera->addPosition(deltaPos);
	/*m_camera->addZoom(-data.m_deltaY);
	if (m_camera->getZoom() < 1.0f)
		m_camera->setZoom(1.0f);*/
	return true;
}