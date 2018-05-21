#include "EditorFunctions.h"

#ifdef _WINDLL
#include "main.h"
#include "CGraphics.h"
#include "src/Maths/VectorMath.h"

#include "Resources/ResourceManager.h"
#include "ScreenManager.h"
#include "Physics/Physics.h"
#include "InputManager/InputManager.h"
#include "GameplayScreen.h"
#include "Resources/MaterialList.h"

#include "Gameplay/GameLogic.h"
#include "Scene/SceneImpl.h"
#include "Gameplay/Level.h"
#include "Gameplay/Platform.h"
#include "Scene/Skybox.h"
#include "EditorCameraController.h"

#include "Scene/InstancingNode.h"
#include "Scene/Camera.h"

#include <Windows.h>
#include <windowsx.h>

IDXGISwapChain* g_pSwapChain;
ID3D11RenderTargetView* g_pRenderTargetView;
ID3D11Texture2D* g_pDepthBuffer;

ID3D11RasterizerState* g_pRState = 0;
ID3D11DepthStencilState* g_pDState = 0;
ID3D11DepthStencilView* g_pDepthView = 0;
ID3D11Debug* g_pDebug;

GameLogic* g_pLogic = 0;
Scene* g_pScene = 0;
Level* g_pLevel = 0;
LookAtCamera* g_pCam = 0;
EditorCameraController* g_pCamControl = 0;

bool g_PlatformResizing = false;

int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;

int SHADOWMAP_RESOLUTION = 4096;

int curX = 0;
int curY = 0;

GameObject* selectedObject;

class Resizer : public IMouseInputHandler {
public:
	int objectIndex;
	PlaneType intersectionPlane;
	bool deforming;
	Vector3 oldPos;

	Resizer() {
		objectIndex = -1;
		intersectionPlane = PlaneNone;
		deforming = false;
	}

	bool HandleMButtonEvent(const MButtonInputData& data) {
		deforming = false;

		if (g_PlatformResizing) {
			if (data.m_button == MButtonLeft) {
				if (data.m_state) {
					if (objectIndex > -1)
						deforming = true;
				}
				else {
					if (objectIndex > -1) {
						PlatformData* objData = static_cast<PlatformData*>(g_pLevel->ObjectByIndex(objectIndex).get());
						GameObjectPtr object = g_pLogic->GetObject(g_pLogic->GetObjectID(objData->GetName()));
						PhysicsBody* body = object->GetPhysicsBody();
						PhysicsShape* shape = body->getCollisionShape();
						delete shape;
						body->setCollisionShape(objData->MakeShape());
					}
				}
			}
		}
		return false;
	}

	bool HandleMMotionEvent(const MMotionInputData& data) {
		if (g_PlatformResizing) {
			if (!deforming) {
				Ray mouseRay;
				GetMouseRay(&mouseRay);
				ObjectIterator it = g_pLevel->ObjectsBegin();
				ObjectIterator end = g_pLevel->ObjectsEnd();

				float minDistance = 0.0f;
				int index = 0;
				objectIndex = -1;
				while (it != end) {
					if (FLAG_IS_PLATFORM((*it)->getFlag())) {
						PlatformData* objData = static_cast<PlatformData*>(it->get());
						Ray secRay;
						Quaternion q = objData->GetRotation().Conjugate();

						secRay.position = MatrixFromQuaternion(q) * (mouseRay.position - objData->GetPosition());
						secRay.position += objData->GetPosition();
						secRay.direction = MatrixFromQuaternion(q) * mouseRay.direction;
						secRay.direction = secRay.direction.Normalize();
						BoundingBox bBox;
						bBox.origin = objData->GetPosition();
						bBox.halfSize = objData->bb.halfSize * TILE_SCALING / 2.0f;
						if (objData->rotation == West || objData->rotation == East) {
							float temp = bBox.halfSize.x;
							bBox.halfSize.x = bBox.halfSize.z;
							bBox.halfSize.z = temp;
						}

						float distance;
						PlaneType intPlane = RayIntersectsBoundingBox(mouseRay, bBox, &distance);
						std::string debug = std::to_string(distance) + '\n';
						OutputDebugStringA(debug.c_str());
						if (intPlane != PlaneNone) {
							if (distance < minDistance || objectIndex == -1) {
								minDistance = distance;
								objectIndex = index;
								intersectionPlane = intPlane;
								if (objData->rotation == North || objData->rotation == South) {
									oldPos = objData->bb.origin + objData->bb.halfSize - Vector3(1.0f, 1.0f, 1.0f);
								}
								else {
									oldPos = objData->bb.origin + objData->bb.halfSize.SwapXZ() - Vector3(1.0f, 1.0f, 1.0f);
								}
							}
						}
					}
					index++;
					++it;
				}
			}
			else {
#define M_PI 3.14159265f
#define IS_SIDE(rot) (rot == East || rot == West)
				PlatformData* objData = static_cast<PlatformData*>(g_pLevel->ObjectByIndex(objectIndex).get());
				BoundingBox& bb = objData->bb;

				float scale = 0.01f;
				int deltaY = -data.m_deltaY;

				float& halfX = (IS_SIDE(objData->rotation)) ? bb.halfSize.z : bb.halfSize.x;
				float& halfZ = (IS_SIDE(objData->rotation)) ? bb.halfSize.x : bb.halfSize.z;

				Vector3 camPos = g_pCam->getPosition();
				if (intersectionPlane == PlaneRight) {
					/*float cosRes = cosf(g_pCam->getRotationY() * M_PI / 180.0f);
					float sinRes = sinf(g_pCam->getRotationY() * M_PI / 180.0f);

					float y = (camPos.y >= bb.origin.y) ? deltaY : -deltaY;
					Vector3 origChange;
					origChange.x = 1.0f;// ((float(data.m_deltaX) * scale) * cosRes - (y * scale) * sinRes);
					origChange.y = 0.0f;
					origChange.z = 0.0f;
					Quaternion q = objData->GetRotation();
					origChange = MatrixFromQuaternion(q) * origChange;
					origChange *= ((float(data.m_deltaX) * scale) * cosRes - (y * scale) * sinRes);

					bb.origin += origChange;
					bb.halfSize.x += ((float(data.m_deltaX) * scale) * cosRes - (y * scale) * sinRes);
					//bb.halfSize.x += ((float(data.m_deltaX) * scale) * cosRes - (y * scale) * sinRes);*/
					Ray mouseRay;
					GetMouseRay(&mouseRay);
					Plane p;
					p.normal = VectorUp;
					p.point = Vector3(0.0f, bb.origin.y * TILE_SCALING, 0.0f);
					float d = RayDistanceToPlane(mouseRay, p);
					Vector3 endRay = mouseRay.position + mouseRay.direction * d;
					float x = floorf(endRay.x);
					halfX = x - bb.origin.x + 1;
					if (halfX < 1)
						halfX = 1;
				}
				else if (intersectionPlane == PlaneLeft) {
					Ray mouseRay;
					GetMouseRay(&mouseRay);
					Plane p;
					p.normal = VectorUp;
					p.point = Vector3(0.0f, bb.origin.y * TILE_SCALING, 0.0f);
					float d = RayDistanceToPlane(mouseRay, p);
					Vector3 endRay = mouseRay.position + mouseRay.direction * d;
					float x = floorf(endRay.x);
					halfX = oldPos.x - x + 1;
					bb.origin.x = x;
					if (bb.origin.x > oldPos.x)
						bb.origin.x = oldPos.x;

					if (halfX < 1)
						halfX = 1;
				}
				else if (intersectionPlane == PlaneFront) {
					/*float cosRes = cosf(g_pCam->getRotationY() * M_PI / 180.0f);
					float sinRes = sinf(g_pCam->getRotationY() * M_PI / 180.0f);

					float y = (camPos.y >= bb.origin.y) ? -deltaY : deltaY;
					Vector3 origChange;
					origChange.x = 0.0f;// ((float(data.m_deltaX) * scale) * cosRes - (y * scale) * sinRes);
					origChange.y = 0.0f;
					origChange.z = 1.0f;
					Quaternion q = objData->GetRotation();
					origChange = MatrixFromQuaternion(q) * origChange;
					origChange *= ((y * scale) * cosRes - (float(data.m_deltaX) * scale) * sinRes);

					bb.origin += origChange;
					bb.halfSize.z += ((y * scale) * cosRes - (float(data.m_deltaX) * scale) * sinRes);*/
					Ray mouseRay;
					GetMouseRay(&mouseRay);
					Plane p;
					p.normal = VectorUp;
					p.point = Vector3(0.0f, bb.origin.y * TILE_SCALING, 0.0f);
					float d = RayDistanceToPlane(mouseRay, p);
					Vector3 endRay = mouseRay.position + mouseRay.direction * d;
					float x = floorf(endRay.z);
					halfZ = x - bb.origin.z + 1;
					if (halfZ < 1)
						halfZ = 1;
				}
				else if (intersectionPlane == PlaneBack) {
					/*float cosRes = cosf(g_pCam->getRotationY() * M_PI / 180.0f);
					float sinRes = sinf(g_pCam->getRotationY() * M_PI / 180.0f);

					float y = (camPos.y >= bb.origin.y) ? -deltaY : deltaY;
					Vector3 origChange;
					origChange.x = 0.0f;// ((float(data.m_deltaX) * scale) * cosRes - (y * scale) * sinRes);
					origChange.y = 0.0f;
					origChange.z = 1.0f;
					Quaternion q = objData->GetRotation();
					origChange = MatrixFromQuaternion(q) * origChange;
					origChange *= ((y * scale) * cosRes - (float(data.m_deltaX) * scale) * sinRes);

					bb.origin += origChange;
					bb.halfSize.z -= ((y * scale) * cosRes - (float(data.m_deltaX) * scale) * sinRes);*/
					Ray mouseRay;
					GetMouseRay(&mouseRay);
					Plane p;
					p.normal = VectorUp;
					p.point = Vector3(0.0f, bb.origin.y * TILE_SCALING, 0.0f);
					float d = RayDistanceToPlane(mouseRay, p);
					Vector3 endRay = mouseRay.position + mouseRay.direction * d;
					float x = floorf(endRay.z);
					halfZ = oldPos.z - x + 1;
					bb.origin.z = x;
					if (bb.origin.z > oldPos.z)
						bb.origin.z = oldPos.z;
					if (halfZ < 1)
						halfZ = 1;
				}
				else if (intersectionPlane == PlaneTop) {
					/*bb.origin.y += (float(deltaY) * scale);
					bb.halfSize.y += (float(deltaY) * scale);*/
					Ray mouseRay;
					GetMouseRay(&mouseRay);
					Vector3 pos;
					GetPositionOnCamPlane(mouseRay, objData->GetPosition(), &pos);
					float y = floorf(pos.y);
					bb.halfSize.y = y - bb.origin.y + 1;
					if (bb.halfSize.y < 1)
						bb.halfSize.y = 1;
				}
				else if (intersectionPlane == PlaneBottom) {
					/*bb.origin.y += (float(deltaY) * scale);
					bb.halfSize.y -= (float(deltaY) * scale);*/
				}

				if (bb.halfSize.x < 1)
					bb.halfSize.x = 1;

				GameObjectPtr object = g_pLogic->GetObject(g_pLogic->GetObjectID(objData->GetName()));
				object->MoveObjectTo(objData->GetPosition());
				PlatformNode* node = static_cast<PlatformNode*>(object->GetSceneNode().get());
				node->bb = bb;
				node->Resize();
			}
		}
		return false;
	}
};
Resizer* g_pResizer;

void SetRenderTarget() {
	g_pDevice->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthView);
}

bool CreateView(int* hwnd) {
	HWND windowHandle = (HWND)hwnd;
	RECT windowRect;
	GetWindowRect(windowHandle, &windowRect);

	SCREEN_WIDTH = windowRect.right - windowRect.left;
	SCREEN_HEIGHT = windowRect.bottom - windowRect.top;

	bool multisample = true;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.

		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = windowHandle;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = (multisample) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (false)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Create the swap chain and the Direct3D device.
	HRESULT result = D3D10CreateDeviceAndSwapChain(NULL, D3D11_DRIVER_TYPE_HARDWARE, NULL, 0, D3D11_SDK_VERSION,
		&swapChainDesc, &g_pSwapChain, &g_pDevice);
	if (FAILED(result))
	{
		MessageBoxW(NULL, L"Could not create device", L"Error", MB_OK);
		return false;
	}


	ID3D11Texture2D* backBufferPtr;
	// Get the pointer to the back buffer.
	result = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = g_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &g_pRenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = SCREEN_WIDTH;
	depthBufferDesc.Height = SCREEN_HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = (multisample) ? 4 : 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = g_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &g_pDepthBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	g_pDevice->CreateRasterizerState(&rasterDesc, &g_pRState);

	g_pDevice->RSSetState(g_pRState);

	g_pDevice->RSSetViewports(1, &viewport);

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	g_pDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDState);

	g_pDevice->OMSetDepthStencilState(g_pDState, 1);

	// Initailze the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = (multisample) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = g_pDevice->CreateDepthStencilView(g_pDepthBuffer, &depthStencilViewDesc, &g_pDepthView);
	if (FAILED(result))
	{
		return false;
	}

	g_pDevice->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthView);
	SetCurrentDevice(g_pDevice);
	InitGraphics();

	g_pResourceManager = DBG_NEW ResourceManager();

	char execPath[128];
	GetModuleFileNameA(NULL, execPath, 128);
	std::string m_execPath = execPath;
	int pos = m_execPath.find_last_of("/\\");
	m_execPath = m_execPath.substr(0, pos);
	m_execPath += "\\Resources";

	ResourceManager::SetResourcesPath(m_execPath);

	g_pInputManager = DBG_NEW InputManager();
	g_pEventManager = DBG_NEW EventManager();

	//g_pScreenManager = DBG_NEW ScreenManager();
	//g_pScreenManager->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

	g_pMaterials = g_pResourceManager->LoadContent<MaterialList>("Materials.xml");

	g_pPhysics = DBG_NEW PhysicsWrapper();
	g_pPhysics->Initialize();

	g_pResizer = new Resizer();
	g_pInputManager->AddMouseInputHandler(g_pResizer);

	return true;
}

void RenderView() {
	if (g_pLogic)
		g_pLogic->Update();

	g_pInputManager->PollInputEvents();
	g_pEventManager->PollEvents();

	//g_pScreenManager->Update();

	Vector4 color(0.0f, 0.0f, 0.0f, 0.0f);

	g_pDevice->ClearRenderTargetView(g_pRenderTargetView, color.v);
	g_pDevice->ClearDepthStencilView(g_pDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (g_pScene) {
		g_pScene->UpdateData();

		g_pScene->Begin();
		SetRenderTarget();
		SetBackCulling();
		g_pScene->Draw(nullptr, nullptr);
	}

	HRESULT res = g_pSwapChain->Present(1, 0);
	if (FAILED(res)) {
		MessageBoxW(NULL, L"Could not present", L"", MB_OK);
	}
}

void ReleaseView() {
	//g_pScreenManager->Release();
	//delete g_pScreenManager;

	g_pInputManager->RemoveMouseInputHandler(g_pResizer);
	delete g_pResizer;

	g_pLevel->Release();
	delete g_pLevel;

	g_pCamControl->Release();
	delete g_pCamControl;

	g_pLogic->Release();
	delete g_pLogic;

	delete g_pScene;
	delete g_pCam;

	delete g_pInputManager;
	delete g_pEventManager;

	g_pResourceManager->unloadAll();
	delete g_pResourceManager;
	g_pPhysics->Release();
	delete g_pPhysics;

	SetCurrentDevice(0);

	g_pRState->Release();
	g_pDState->Release();
	g_pDepthView->Release();

	g_pDepthBuffer->Release();
	g_pRenderTargetView->Release();
	g_pSwapChain->Release();
	g_pDevice->Release();

}

void ResizeView(int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	g_pDevice->OMSetRenderTargets(0, 0, 0);

	g_pRenderTargetView->Release();
	g_pDepthView->Release();
	g_pDepthBuffer->Release();
	g_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D* backBufferPtr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	g_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &g_pRenderTargetView);
	backBufferPtr->Release();



	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = SCREEN_WIDTH;
	depthBufferDesc.Height = SCREEN_HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = (true) ? 4 : 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	g_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &g_pDepthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = (true) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	g_pDevice->CreateDepthStencilView(g_pDepthBuffer, &depthStencilViewDesc, &g_pDepthView);


	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pDevice->RSSetViewports(1, &viewport);

	g_pCam->setProjection(MatrixMakePerspective(65, float(width) / float(height), 0.1f, 1000.0f));
}

void NewLevel() {
	if (g_pCamControl) {
		g_pCamControl->Release();
		delete g_pCamControl;
	}
	if (g_pLogic) {
		g_pLogic->Release();
		delete g_pLogic;
	}
	if (g_pScene) {
		delete g_pScene;
	}
	if (g_pLevel) {
		g_pLevel->Release();
		delete g_pLevel;
	}

	g_pLevel = new Level;

	g_pScene = new Scene;
	g_pScene->Initialize(Vector3(-1.0f, 0.8f, -0.3f).Normalize());
	g_pCam = new LookAtCamera();
	g_pCam->setProjection(MatrixMakePerspective(65, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 1000.0f));
	g_pCam->setZoom(25.0f);
	g_pCam->setRotationX(-45.0f);
	g_pCam->setRotationY(45.0f);
	g_pScene->SetCamera(g_pCam);

	SkyboxNode* skybox = DBG_NEW SkyboxNode;
	skybox->init();
	g_pScene->AddNode(NODE_PTR(skybox));

	Shader* shader = g_pResourceManager->LoadContent<Shader>("..\\Shader.cso");
	shader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex);
	shader->SetVariable("shadowResolution", &SHADOWMAP_RESOLUTION);
	Shader* shadowShader = g_pResourceManager->LoadContent<Shader>("..\\ShadowShader.cso");
	shadowShader->Initialize("Technique1", "Pass1", VertexInputPos);
	Shader* terrainShader = g_pResourceManager->LoadContent<Shader>("..\\TerrainShader.cso");
	terrainShader->Initialize("Technique1", "Pass1", VertexInputPosNormTex);
	terrainShader->SetVariable("shadowResolution", &SHADOWMAP_RESOLUTION);
	Shader* gateShader = g_pResourceManager->LoadContent<Shader>("..\\GateShader.cso");
	gateShader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex);
	Shader* starShader = g_pResourceManager->LoadContent<Shader>("..\\StarShader.cso");
	starShader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex, InstanceInputWorldPos);
	Shader* instanceShadow = g_pResourceManager->LoadContent<Shader>("..\\InstancedShadow.cso");
	instanceShadow->Initialize("Technique1", "Pass1", VertexInputPos, InstanceInputWorldPos);
	Shader* simpleShader = g_pResourceManager->LoadContent<Shader>("..\\ShaderSimple.cso");
	simpleShader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex);

	g_pScene->AddShader(ShaderTypeModel, shader);
	g_pScene->AddShader(ShaderTypeShadow, shadowShader);
	g_pScene->AddShader(ShaderTypeTerrain, terrainShader);
	g_pScene->AddShader(ShaderTypeGate, gateShader);
	g_pScene->AddShader(ShaderTypeStar, starShader);
	g_pScene->AddShader(ShaderTypeInstanceShadow, instanceShadow);
	g_pScene->AddShader(ShaderTypeSimple, simpleShader);

	float waterHeight = -21.0f;
	g_pScene->SetShaderGlobal("waterHeight", &waterHeight);

	StarInstances* insts = new StarInstances();
	insts->m_model = g_pResourceManager->LoadContent<Model>("Star.obj");
	g_pScene->AddInstancingNode(INST_NODE_PTR(insts));

	g_pLogic = new GameLogic(g_pScene);
	g_pLogic->InitializeFromLevel(g_pLevel);

	if (!g_pLogic->GetObject(g_pLogic->GetObjectID("Respawn"))) {
		MessageBoxW(NULL, L"Adding Respawn", L"Info", MB_OK);
		RespawnData* data = new RespawnData;
		data->position = Vector3(0.0f, 50.0f, 0.0f);
		data->m_name = "Respawn";
		data->m_parentName = "None";
		g_pLevel->AddObject(ObjectDataPtr(data));
		g_pLogic->AddObject(data);
	}

	insts->InitData();

	g_pResourceManager->LoadFromQueue();

	g_pCamControl = new EditorCameraController(g_pCam);
	g_pCamControl->Initialize();

	//level->Release();
	//delete level;
}

void LoadLevel(const char* path) {
	if (g_pCamControl) {
		g_pCamControl->Release();
		delete g_pCamControl;
	}
	if (g_pLogic) {
		g_pLogic->Release();
		delete g_pLogic;
	}
	if (g_pScene) {
		delete g_pScene;
	}
	if (g_pLevel) {
		g_pLevel->Release();
		delete g_pLevel;
	}

	g_pLevel = new Level;
	g_pLevel->LoadLevel(path);

	g_pScene = new Scene;
	g_pScene->Initialize(Vector3(-1.0f, 0.8f, -0.3f).Normalize());
	g_pCam = new LookAtCamera();
	g_pCam->setProjection(MatrixMakePerspective(65, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 1000.0f));
	g_pCam->setZoom(25.0f);
	g_pCam->setRotationX(-45.0f);
	g_pCam->setRotationY(45.0f);
	g_pScene->SetCamera(g_pCam);

	SkyboxNode* skybox = DBG_NEW SkyboxNode;
	skybox->init();
	g_pScene->AddNode(NODE_PTR(skybox));

	Shader* shader = g_pResourceManager->LoadContent<Shader>("..\\Shader.cso");
	shader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex);
	shader->SetVariable("shadowResolution", &SHADOWMAP_RESOLUTION);
	Shader* shadowShader = g_pResourceManager->LoadContent<Shader>("..\\ShadowShader.cso");
	shadowShader->Initialize("Technique1", "Pass1", VertexInputPos);
	Shader* terrainShader = g_pResourceManager->LoadContent<Shader>("..\\TerrainShader.cso");
	terrainShader->Initialize("Technique1", "Pass1", VertexInputPosNormTex);
	terrainShader->SetVariable("shadowResolution", &SHADOWMAP_RESOLUTION);
	Shader* gateShader = g_pResourceManager->LoadContent<Shader>("..\\GateShader.cso");
	gateShader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex);
	Shader* starShader = g_pResourceManager->LoadContent<Shader>("..\\StarShader.cso");
	starShader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex, InstanceInputWorldPos);
	Shader* instanceShadow = g_pResourceManager->LoadContent<Shader>("..\\InstancedShadow.cso");
	instanceShadow->Initialize("Technique1", "Pass1", VertexInputPos, InstanceInputWorldPos);
	Shader* simpleShader = g_pResourceManager->LoadContent<Shader>("..\\ShaderSimple.cso");
	simpleShader->Initialize("Technique1", "Pass1", VertexInputPosTanNormTex);
	//bitmapShader = g_pResourceManager->LoadContent<Shader>("..\\BitmapShader.cso");
	//bitmapShader->Initialize("FontTechnique", "FontPass", VertexInputPosTex);
	Matrix4 ortho = MatrixMakeOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
	//bitmapShader->SetVariable("projectionMatrix", &ortho);

	g_pScene->AddShader(ShaderTypeModel, shader);
	g_pScene->AddShader(ShaderTypeShadow, shadowShader);
	g_pScene->AddShader(ShaderTypeTerrain, terrainShader);
	g_pScene->AddShader(ShaderTypeGate, gateShader);
	g_pScene->AddShader(ShaderTypeStar, starShader);
	g_pScene->AddShader(ShaderTypeInstanceShadow, instanceShadow);
	g_pScene->AddShader(ShaderTypeSimple, simpleShader);

	float waterHeight = -21.0f;
	g_pScene->SetShaderGlobal("waterHeight", &waterHeight);

	StarInstances* insts = new StarInstances();
	insts->m_model = g_pResourceManager->LoadContent<Model>("Star.obj");
	g_pScene->AddInstancingNode(INST_NODE_PTR(insts));

	g_pLogic = new GameLogic(g_pScene);
	g_pLogic->InitializeFromLevel(g_pLevel);

	if (!g_pLogic->GetObject(g_pLogic->GetObjectID("Respawn"))) {
		MessageBoxW(NULL, L"Adding Respawn", L"Info", MB_OK);
		RespawnData* data = new RespawnData;
		data->position = Vector3(0.0f, 50.0f, 0.0f);
		data->m_name = "Respawn";
		data->m_parentName = "None";
		g_pLevel->AddObject(ObjectDataPtr(data));
		g_pLogic->AddObject(data);
	}

	insts->InitData();

	g_pResourceManager->LoadFromQueue();

	g_pCamControl = new EditorCameraController(g_pCam);
	g_pCamControl->Initialize();


	//level->Release();
	//delete level;
}

void SaveLevel(const char* path) {
	g_pLevel->SaveLevel(path);
}

void WndProc(int* hwnd, int msg, int pWParam, int pLParam) {
	HWND handle = (HWND)hwnd;
	static short lastX = 0;
	static short lastY = 0;

	LPARAM lParam = (LPARAM)pLParam;
	WPARAM wParam = (WPARAM)pWParam;

	switch (msg) {
	case WM_KEYDOWN: {
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
						   curX = newX;
						   curY = newY;
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
		break;
	case WM_DESTROY:
		break;
	}
}


const char* PickObject() {
	if (selectedObject)
		selectedObject->GetSceneNode()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));

	Vector3 nearPos(curX, SCREEN_HEIGHT - curY, 0.0f);
	Vector3 farPos(curX, SCREEN_HEIGHT - curY, 1.0f);

	Vector3 nearWorld;
	Vector3 farWorld;

	int viewport[4];
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = SCREEN_WIDTH;
	viewport[3] = SCREEN_HEIGHT;

	Unproject(nearPos, g_pCam->getProjection(), g_pCam->getView(), viewport, &nearWorld);
	Unproject(farPos, g_pCam->getProjection(), g_pCam->getView(), viewport, &farWorld);

	Ray mouseRay;
	mouseRay.position = nearWorld;
	mouseRay.direction = farWorld;

	selectedObject = g_pLogic->ShootRay(mouseRay);

	if (selectedObject) {
		selectedObject->GetSceneNode()->SetColor(Vector4(0.25f, 0.5f, 1.0f, 0.7f));
		return selectedObject->data->m_name.c_str();
	}
	else
		return "\0";
}

GameObject* tempObject;
void PickTempObject() {
	if (tempObject) {
		if (!selectedObject || (selectedObject && selectedObject->GetIdentifier() != tempObject->GetIdentifier()))
			tempObject->GetSceneNode()->SetColor(Vector4(0.0f));
	}

	Vector3 nearPos(curX, SCREEN_HEIGHT - curY, 0.0f);
	Vector3 farPos(curX, SCREEN_HEIGHT - curY, 1.0f);

	Vector3 nearWorld;
	Vector3 farWorld;

	int viewport[4];
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = SCREEN_WIDTH;
	viewport[3] = SCREEN_HEIGHT;

	Unproject(nearPos, g_pCam->getProjection(), g_pCam->getView(), viewport, &nearWorld);
	Unproject(farPos, g_pCam->getProjection(), g_pCam->getView(), viewport, &farWorld);

	Ray mouseRay;
	mouseRay.position = nearWorld;
	mouseRay.direction = farWorld;

	tempObject = g_pLogic->ShootRay(mouseRay);
	if (tempObject) {
		if (!selectedObject || (selectedObject && selectedObject->GetIdentifier() != tempObject->GetIdentifier()))
		tempObject->GetSceneNode()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.3f));
	}
}

MaterialList::MaterialMap::iterator mIt;
bool MaterialsBegin(EditorMaterial* mat) {
	mIt = g_pMaterials->m_materials.begin();
	if (mIt != g_pMaterials->m_materials.end()) {
		mat->name = mIt->second.m_name.c_str();
		mat->diffuseMap = mIt->second.m_diffuseMap.c_str();
		mat->normalMap = mIt->second.m_normalMap.c_str();
		return true;
	}
	return false;
}

bool MaterialsNext(EditorMaterial* mat) {
	mIt++;
	if (mIt != g_pMaterials->m_materials.end()) {
		mat->name = mIt->second.m_name.c_str();
		mat->diffuseMap = mIt->second.m_diffuseMap.c_str();
		mat->normalMap = mIt->second.m_normalMap.c_str();
		return true;
	}
	return false;
}

void GetObjectDataByName(const char* name, ObjectData* outData) {
	GameObjectPtr object = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	outData->flag = object->data->getFlag();
	outData->position = object->data->GetPositionEditor();
	Quaternion q = object->GetSceneNode()->GetOrientation();
	outData->rotation.y = atan2f(2.0f * q.y * q.w - 2 * q.x * q.z, 1 - 2 * q.y * q.y - 2 * q.z * q.z) * 180.0f / M_PI;
	outData->rotation.z = asinf(2 * q.x * q.y + 2 * q.z * q.w) * 180.0f / M_PI;
	outData->rotation.x = atan2f(2.0f * q.x * q.w - 2.0f * q.y * q.z, 1 - 2 * q.x * q.x - 2 * q.z * q.z) * 180.0f / M_PI;
	outData->parent = object->data->m_parentName.c_str();
}

void SetPositionForObject(const char* name, Vector3 position) {
	GameObjectPtr object = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	if (FLAG_IS_PLATFORM(object->data->getFlag())) {
		Vector3 pos(floorf(position.x), floorf(position.y), floorf(position.z));
		object->data->SetPosition(pos);
		object->MoveObjectTo(object->data->GetPosition());
	}
	else {
		object->data->SetPosition(position);
		object->MoveObjectTo(position);
		object->GetSceneNode()->SetOrigin(position);
	}
}

void AddRotationForObject(const char* name, Quaternion q) {
	GameObjectPtr object = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	Quaternion rot = object->GetSceneNode()->GetOrientation() * q;
	object->data->SetRotation(rot);
	object->GetSceneNode()->SetOrientation(rot);
	object->GetPhysicsBody()->getWorldTransform().setRotation(rot);
}

void AddRotationForPlatform(const char* name, int direction) {
	GameObjectPtr object = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	if (FLAG_IS_PLATFORM(object->data->getFlag())) {
		PlatformData* data = (PlatformData*)object->data;
		data->rotation = (PlatformRotation)((data->rotation + direction) % 4);
		object->GetSceneNode()->SetOrigin(data->GetPosition());
		object->MoveObjectTo(data->GetPosition());
		object->GetSceneNode()->SetOrientation(data->GetRotation());
		object->GetPhysicsBody()->getWorldTransform().setRotation(data->GetRotation());
	}
}

void GetMouseRay(_out_ Ray* ray) {
	Vector3 nearPos(curX, SCREEN_HEIGHT - curY, 0.0f);
	Vector3 farPos(curX, SCREEN_HEIGHT - curY, 1.0f);

	Vector3 nearWorld;
	Vector3 farWorld;

	int viewport[4];
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = SCREEN_WIDTH;
	viewport[3] = SCREEN_HEIGHT;

	Unproject(nearPos, g_pCam->getProjection(), g_pCam->getView(), viewport, &nearWorld);
	Unproject(farPos, g_pCam->getProjection(), g_pCam->getView(), viewport, &farWorld);

	ray->position = nearWorld;
	ray->direction = (farWorld - nearWorld).Normalize();
}

void GetPositionOnPlane(Ray ray, float y, _out_ Vector3* position) {
	Plane plane;
	plane.normal = VectorUp;
	plane.point = VectorUp * y;

	float distance = RayDistanceToPlane(ray, plane);
	*position = ray.position + ray.direction * distance;
}

void GetPositionOnCamPlane(Ray ray, Vector3 pos, _out_ Vector3* outPosition) {
	Vector3 camPos = g_pCam->getPosition();
	Vector3 temp(pos.x, camPos.y, pos.z);
	Vector3 direction = (temp - camPos).Normalize();
	direction *= -1.0f;

	Plane plane;
	plane.point = temp;
	plane.point.y = 0.0f;
	plane.normal = direction;

	float distance = RayDistanceToPlane(ray, plane);
	*outPosition = ray.position + ray.direction * distance;
}

const char* GetWorldPosition(Ray ray, _out_ Vector3* outPosition) {
	ray.direction = ray.position + ray.direction * 1000.0f;
	GameObject* object = 0;
	*outPosition = g_pLogic->ShootRayPosition(ray, &object);
	return (object) ? object->data->m_name.c_str() : "None";
}

const char* AddObject(Vector3 position, char flag, const char* parent) {
	switch (flag) {
		case STAR_FLAG:
		{
						  StarData* data = new StarData();
						  data->position = position;
						  int index = 0;
						  do {
							  data->m_name = "Star" + std::to_string(index);
							  index++;
						  } while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

						  data->m_parentName = "None";
						  ObjectDataPtr objectData(data);
						  g_pLevel->AddObject(objectData);
						  GameObjectPtr object = g_pLogic->AddObject(data);
						  return object->m_name.c_str();
		}
		case BUTTON_FLAG:
		{
							ButtonData* data = new ButtonData();
							data->m_position = position;
							data->m_position.y -= 1.0f;
							data->m_position.y += 0.1903f / 2.0f - 0.05f;
							int index = 0;
							do {
								data->m_name = "Button" + std::to_string(index);
								index++;
							} while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

							data->m_parentName = "None";
							//data->m_position -= g_pLogic->GetObject(g_pLogic->GetObjectID(parent))->GetAbsolutePosition();
							ObjectDataPtr objectData(data);
							g_pLevel->AddObject(objectData);
							GameObjectPtr object = g_pLogic->AddObject(data);
							return object->m_name.c_str();
		}
		case PLATFORM_FLAG:
		{
							PlatformData* data = new PlatformData();
							data->bb.origin = Vector3(floorf(position.x), floorf(position.y - 0.9f), floorf(position.z));
							data->bb.halfSize = Vector3(1.0f, 1.0f, 1.0f);
							int index = 0;
							do {
								data->m_name = "Platform" + std::to_string(index);
								index++;
							} while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

							data->m_parentName = "None";
							strcpy(data->material, "Wood");
							data->rotation = North;
							ObjectDataPtr objectData(data);
							g_pLevel->AddObject(objectData);
							GameObjectPtr object = g_pLogic->AddObject(data);
							return object->m_name.c_str();
		}
		case RAMP_FLAG:
		{
						  RampData* data = new RampData();
						  data->bb.origin = Vector3(floorf(position.x), floorf(position.y - 0.9f), floorf(position.z));
						  data->bb.halfSize = Vector3(1.0f, 1.0f, 1.0f);
						  int index = 0;
						  do {
							  data->m_name = "Ramp" + std::to_string(index);
							  index++;
						  } while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

						  data->m_parentName = "None";
						  strcpy(data->material, "Wood");
						  data->rotation = North;
						  ObjectDataPtr objectData(data);
						  g_pLevel->AddObject(objectData);
						  GameObjectPtr object = g_pLogic->AddObject(data);
						  return object->m_name.c_str();
		}

		case CORNER_FLAG:
		{
							CornerData* data = new CornerData();
							data->bb.origin = Vector3(floorf(position.x), floorf(position.y - 0.9f), floorf(position.z));
							data->bb.halfSize = Vector3(1.0f, 1.0f, 1.0f);
							int index = 0;
							do {
								data->m_name = "Corner" + std::to_string(index);
								index++;
							} while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

							data->m_parentName = "None";
							strcpy(data->material, "Wood");
							data->rotation = North;
							ObjectDataPtr objectData(data);
							g_pLevel->AddObject(objectData);
							GameObjectPtr object = g_pLogic->AddObject(data);
							return object->m_name.c_str();
		}
		case TELEPORTER_FLAG:
		{
								TeleporterData* data = new TeleporterData();
								data->m_position = position;
								data->m_position.y += 0.42073f;
								int index = 0;
								do {
									data->m_name = "Tel" + std::to_string(index);
									index++;
								} while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

								data->m_parentName = "None";
								data->m_target = "None";
								ObjectDataPtr objectData(data);
								g_pLevel->AddObject(objectData);
								GameObjectPtr object = g_pLogic->AddObject(data);
								return object->m_name.c_str();
		}
		case POWERUP_FLAG:
		{
							 PowerUpData* data = new PowerUpData();
							 data->position = position;
							 int index = 0;
							 do {
								 data->m_name = "Power" + std::to_string(index);
								 index++;
							 } while (g_pLogic->GetObject(g_pLogic->GetObjectID(data->m_name)) != nullptr);

							 data->m_parentName = "None";
							 ObjectDataPtr objectData(data);
							 g_pLevel->AddObject(objectData);
							 GameObjectPtr object = g_pLogic->AddObject(data);
							 return object->m_name.c_str();
		}
	}

	return nullptr;
}

void RemoveObjectByName(const char* name) {
	GameObjectPtr gameObj = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	g_pLogic->RemoveObject(g_pLogic->GetObjectID(name));

	ObjectIterator it = g_pLevel->ObjectsBegin();
	ObjectIterator end = g_pLevel->ObjectsEnd();

	while (it != end) {
		if (it->get() == gameObj->data) {
			g_pLevel->m_objects.erase(it);
			break;
		}
		++it;
	}
}

void SetPlatformResizing(bool resizing) {
	g_PlatformResizing = resizing;
}

GameLogic::ObjectMap::iterator objIt;

bool ObjectsBegin(_out_ const char** name) {
	objIt = g_pLogic->m_objects.begin();
	if (objIt != g_pLogic->m_objects.end()) {
		*name = objIt->second->m_name.c_str();
		return true;
	}

	return false;
}

bool ObjectsNext(_out_ const char** name) {
	objIt++;
	if (objIt != g_pLogic->m_objects.end()) {
		*name = objIt->second->m_name.c_str();
		return true;
	}

	return false;
}

void GetPlatformData(const char* name, _out_ Vector3* size) {
	GameObjectPtr gameObj = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	LevelObjectData* data = gameObj->data;
	if (FLAG_IS_PLATFORM(data->getFlag())) {
		PlatformData* platData = (PlatformData*)data;
		size->x = platData->bb.halfSize.x;
		size->y = platData->bb.halfSize.y;
		size->z = platData->bb.halfSize.z;
	}
}

void SetPlatformMaterial(const char* name, const char* material) {
	GameObjectPtr gameObj = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	LevelObjectData* data = gameObj->data;
	if (FLAG_IS_PLATFORM(data->getFlag())) {
		PlatformData* platData = (PlatformData*)data;
		strcpy(platData->material, material);
		PlatformNode* node = static_cast<PlatformNode*>(gameObj->GetSceneNode().get());
		node->mat = g_pMaterials->GetVisualMaterial(material);
	}
}

void SetNameForObject(const char* name, const char* newName) {
	GameObjectPtr gameObj = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	g_pLogic->RemoveObject(gameObj);
	gameObj->data->m_name = newName;
	GameObjectPtr obj = g_pLogic->AddObject(gameObj->data);
	obj->data = gameObj->data;
}

void SetTargetForTeleporter(const char* name, const char* target) {
	GameObjectPtr gameObj = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	LevelObjectData* data = gameObj->data;
	if (data->getFlag() == TELEPORTER_FLAG) {
		TeleporterData* telData = static_cast<TeleporterData*>(data);
		telData->m_target = target;
	}
}

void GetTeleporterData(const char* name, _out_ const char** outTarget) {
	GameObjectPtr gameObj = g_pLogic->GetObject(g_pLogic->GetObjectID(name));
	LevelObjectData* data = gameObj->data;
	if (data->getFlag() == TELEPORTER_FLAG) {
		TeleporterData* telData = static_cast<TeleporterData*>(data);
		*outTarget = telData->m_target.c_str();
	}
}

char tempFlag;
SceneNodePtr temp = nullptr;
void AddTemporaryObject(char flag) {
	tempFlag = flag;
	switch (flag) {
	case STAR_FLAG:
	{
					  StarData data;
					  temp = NODE_PTR(data.MakeSceneNode());
					  g_pScene->AddNode(temp);
					  break;
	}
	case BUTTON_FLAG:
	{
						ButtonData data;
						temp = NODE_PTR(data.MakeSceneNode());
						g_pScene->AddNode(temp);
						break;
	}
	case PLATFORM_FLAG:
	{
						  PlatformData data;
						  strcpy(data.material, "Wood");
						  data.bb.halfSize = Vector3(1.0f, 1.0f, 1.0f);
						  temp = NODE_PTR(data.MakeSceneNode());
						  g_pScene->AddNode(temp);
						  break;
	}
	case RAMP_FLAG:
	{
					  RampData data;
					  strcpy(data.material, "Wood");
					  data.bb.halfSize = Vector3(1.0f, 1.0f, 1.0f);
					  temp = NODE_PTR(data.MakeSceneNode());
					  g_pScene->AddNode(temp);
					  break;
	}

	case CORNER_FLAG:
	{
						CornerData data;
						strcpy(data.material, "Wood");
						data.bb.halfSize = Vector3(1.0f, 1.0f, 1.0f);
						temp = NODE_PTR(data.MakeSceneNode());
						g_pScene->AddNode(temp);
						break;
	}
	case TELEPORTER_FLAG:
	{
							TeleporterData data;
							temp = NODE_PTR(data.MakeSceneNode());
							g_pScene->AddNode(temp);
							break;
	}
	case POWERUP_FLAG:
	{
						 PowerUpData data;
						 temp = NODE_PTR(data.MakeSceneNode());
						 g_pScene->AddNode(temp);
						 break;
	}
	}
}

void RemoveTemp() {
	if (temp) {
		g_pScene->RemoveNode(temp);
		temp = nullptr;
		tempFlag = '\0';
	}
}

void SetTempPosition(Vector3 position) {
	if (temp) {
		Vector3 pos = position;
		if (FLAG_IS_PLATFORM(tempFlag)) {
			pos.x = TILE_SCALING / 2.0f + floorf(pos.x);
			pos.y = TILE_SCALING / 2.0f + floorf(pos.y);
			pos.z = TILE_SCALING / 2.0f + floorf(pos.z);
		}
		if (tempFlag == STAR_FLAG)
			pos.y += 1.0f;
		if (tempFlag == TELEPORTER_FLAG)
			pos.y += 1.42073f;
		if (tempFlag == POWERUP_FLAG)
			pos.y += 1.0f;
		if (tempFlag == BUTTON_FLAG)
			pos.y += 0.1903f / 2.0f - 0.05f;
		temp->SetOrigin(pos);
	}
}

#endif