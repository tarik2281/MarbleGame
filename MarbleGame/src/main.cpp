#ifdef _DEBUG
//#include <vld.h>
#endif

#include <Windows.h>
#include <stdio.h>
#include <string>
#include <windowsx.h>

#include "CGraphics.h"
#include "DXShader.h"
#include "Resources/Model.h"
#include "Resources/ResourceManager.h"
#include "EventManager/EventManager.h" 
#include "InputManager/InputManager.h"
#include "ScreenManager.h"
#include "main.h"
#include "Gameplay/CameraController.h"
#include "Scene/Camera.h"
#include "Physics/Physics.h"
#include "Gameplay/Level.h"
#include "Resources/MaterialList.h"
#include "GameplayScreen.h"

#include "Scene/InstancingNode.h"

#include "UserInterface/Font.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxgi.lib")

ResourceManager* g_pResourceManager = 0;
EventManager* g_pEventManager = 0;
InputManager* g_pInputManager = 0;
ScreenManager* g_pScreenManager = 0;
PhysicsWrapper* g_pPhysics = 0;
MaterialList* g_pMaterials = 0;

#ifndef _WINDLL
ID3D11DepthStencilState* g_pDState = 0;

int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;

int SHADOWMAP_RESOLUTION = 2048;
int NUM_CASCADES = 4;

float g_fFrameRate = 0;
float g_fMaxFPS = 60.0f;

bool g_running = true;


LRESULT APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);

void GetScreenSize() {
	RECT screenRect;
	HWND desktopWnd = GetDesktopWindow();
	GetWindowRect(desktopWnd, &screenRect);
	SCREEN_WIDTH = screenRect.right;
	SCREEN_HEIGHT = screenRect.bottom;
}

int APIENTRY
WinMain(
    HINSTANCE hCurrentInst,
    HINSTANCE hPreviousInst,
    LPSTR lpszCmdLine,
    int nCmdShow)
{
#ifdef _DEBUG
	//VLDEnable(); 
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	bool multisample = true;
	bool fullscreen = true;

	//SHADOWMAP_RESOLUTION = 1024;

	// MarbleGame.exe [ShadowMapResolution] [NumCascades] [AntiAliasing] [Fullscreen]

	if (__argc > 1) {
		SHADOWMAP_RESOLUTION = atoi(__argv[1]);
	}

	if (__argc > 2) {
		NUM_CASCADES = atof(__argv[2]);
	}

	if (__argc > 3) {
		multisample = atoi(__argv[3]);
	}

	if (__argc > 4) {
		fullscreen = atoi(__argv[4]);
	}

	GetScreenSize();

	WNDCLASSEXW wc;
	HWND hwnd;
	MSG msg;

	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hCurrentInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"CLASSNAME";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassExW(&wc)) {
		MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"CLASSNAME", L"MarbleGame",
		WS_OVERLAPPEDWINDOW | CS_OWNDC, CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL,
		hCurrentInst, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	g_pGraphics = new CGraphics();
	g_pGraphics->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, hwnd, CNumSamples4);

	g_pResourceManager = DBG_NEW ResourceManager();

	char execPath[128];
	GetModuleFileNameA(NULL, execPath, 128);
	std::string m_execPath = execPath;
	int pos = m_execPath.find_last_of("/\\");
	m_execPath = m_execPath.substr(0, pos);
	m_execPath += "\\Resources";

	ResourceManager::SetResourcesPath(m_execPath);
	CEffectManager::SetShadersPath(m_execPath + "\\Shaders\\");

	g_pInputManager = DBG_NEW InputManager();
	g_pEventManager = DBG_NEW EventManager();

	g_pScreenManager = DBG_NEW ScreenManager();
	g_pScreenManager->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

	g_pMaterials = g_pResourceManager->LoadContent<MaterialList>("Materials.xml");

	g_pPhysics = DBG_NEW PhysicsWrapper();
	g_pPhysics->Initialize();

	GameplayScreen* screen = DBG_NEW GameplayScreen();
	screen->SetNormalMapping(true);
	g_pScreenManager->AddScreen(ScreenPtr(screen));


	unsigned long long frames = 0;
	unsigned int oldTime = 0;
	unsigned int currentTime = 0;
	std::string title;

	while (g_running) {
		frames++;
		currentTime = GetTickCount();
		unsigned int elapsedTime = currentTime - oldTime;
		if (elapsedTime >= 1000) {
			// if (frames > 60) frames = 60;
			g_fFrameRate = max(1.0f / (float)frames, 1.0f / 60.0f);
			title = "FPS: " + std::to_string(frames);
			SetWindowTextA(hwnd, title.c_str());
			frames = 0;
			oldTime = currentTime;
		}

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		g_pInputManager->PollInputEvents();
		g_pEventManager->PollEvents();

		g_pScreenManager->Update();

		g_pGraphics->ClearBackBuffer();

		g_pScreenManager->Render();

		g_pGraphics->SwapBuffers();
	}

	g_pScreenManager->Release();
	delete g_pScreenManager;

	delete g_pInputManager;
	delete g_pEventManager;

	g_pResourceManager->unloadAll();
	delete g_pResourceManager;
	g_pPhysics->Release();
	delete g_pPhysics;

	g_pGraphics->Release();
	
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	//VLDReportLeaks();

	return msg.wParam;
}

LRESULT APIENTRY WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static short lastX = 0;
	static short lastY = 0;

	switch (msg) {
	case WM_KEYDOWN: {
						 if ((HIWORD(lParam) & KF_REPEAT) != 0)
							 break;
						 if (wParam == VK_ESCAPE) {
							 DestroyWindow(hwnd);
							 break;
						 }
						 if (!g_pInputManager)
							 break;
						 KeyInputData* data = DBG_NEW KeyInputData(wParam, NULL, true);
						 g_pInputManager->PushInputEvent(IInputDataPtr(data));
						 break;
	}
	case WM_KEYUP: {

					   if (!g_pInputManager)
						   break;
					   KeyInputData* data = DBG_NEW KeyInputData(wParam, NULL, false);
					   g_pInputManager->PushInputEvent(IInputDataPtr(data));
					   break;
	}
	case WM_LBUTTONDOWN: {

							 if (!g_pInputManager)
								 break;
							 Modifier mod = 0;
							 if (wParam & MK_CONTROL)
								 mod |= MODIFIER_CTRL;
							 if (wParam & MK_SHIFT)
								 mod |= MODIFIER_SHIFT;
							 MButtonInputData* data = DBG_NEW MButtonInputData(MButtonLeft, mod, true);
							 g_pInputManager->PushInputEvent(IInputDataPtr(data));
							 break;
	}
	case WM_LBUTTONUP: {

						   if (!g_pInputManager)
							   break;
						   Modifier mod = 0;
						   if (wParam & MK_CONTROL)
							   mod |= MODIFIER_CTRL;
						   if (wParam & MK_SHIFT)
							   mod |= MODIFIER_SHIFT;
						   MButtonInputData* data = DBG_NEW MButtonInputData(MButtonLeft, 0, false);
						   g_pInputManager->PushInputEvent(IInputDataPtr(data));
						   break;
	}
	case WM_RBUTTONDOWN: {

							 if (!g_pInputManager)
								 break;
							 Modifier mod = 0;
							 if (wParam & MK_CONTROL)
								 mod |= MODIFIER_CTRL;
							 if (wParam & MK_SHIFT)
								 mod |= MODIFIER_SHIFT;
							 MButtonInputData* data = DBG_NEW MButtonInputData(MButtonRight, mod, true);
							 g_pInputManager->PushInputEvent(IInputDataPtr(data));
							 break;
	}
	case WM_RBUTTONUP: {

						   if (!g_pInputManager)
							   break;
						   Modifier mod = 0;
						   if (wParam & MK_CONTROL)
							   mod |= MODIFIER_CTRL;
						   if (wParam & MK_SHIFT)
							   mod |= MODIFIER_SHIFT;
						   MButtonInputData* data = DBG_NEW MButtonInputData(MButtonRight, 0, false);
						   g_pInputManager->PushInputEvent(IInputDataPtr(data));
						   break;
	}
	case WM_MOUSEMOVE: {

						   if (!g_pInputManager)
							   break;
						   short newX = GET_X_LPARAM(lParam);
						   short newY = GET_Y_LPARAM(lParam);
						   if (newX == lastX && newY == lastY)
							   break;
						   MMotionInputData* data = DBG_NEW MMotionInputData(newX, newY, newX - lastX, newY - lastY);
						   g_pInputManager->PushInputEvent(IInputDataPtr(data));
						   lastX = newX;
						   lastY = newY;
						   break;
	}
	case WM_MOUSEWHEEL: {

							if (!g_pInputManager)
								break;
							float delta = float(GET_WHEEL_DELTA_WPARAM(wParam)) / 60.0f;
							MScrollInputData* data = DBG_NEW MScrollInputData(0.0f, float(delta));
							g_pInputManager->PushInputEvent(IInputDataPtr(data));
							break;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		g_running = false;
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

#endif