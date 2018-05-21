#include "CGraphics.h"
#include "Maths/VectorMath.h"

CGraphics* g_pGraphics = 0;

#ifndef _DEBUG
#define DEVICE_CREATE_FLAG 0
#else
#define DEVICE_CREATE_FLAG D3D11_CREATE_DEVICE_DEBUG
#endif

void CGraphics::Initialize(int width, int height, HWND hwnd, CNumSamples numSamples) {
	m_numSamples = numSamples;
	m_width = width;
	m_height = height;

	IDXGIFactory* pFactory;
	IDXGIAdapter* pAdapter;
	IDXGIOutput* pAdapterOutput;
	unsigned int numModes;
	unsigned int numerator = 0;
	unsigned int denominator = 1;
	DXGI_MODE_DESC* pDisplayModes;

	// Get refresh rate
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	pFactory->EnumAdapters(0, &pAdapter);
	pAdapter->EnumOutputs(0, &pAdapterOutput);
	pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	
	pDisplayModes = DBG_NEW DXGI_MODE_DESC[numModes];
	pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, pDisplayModes);

	for (int i = 0; i < numModes; i++) {
		if (pDisplayModes[i].Width == m_width && pDisplayModes[i].Height == m_height) {
			numerator = pDisplayModes[i].RefreshRate.Numerator;
			denominator = pDisplayModes[i].RefreshRate.Denominator;
		}
	}

	delete[] pDisplayModes;
	pDisplayModes = nullptr;
	DX_SAFE_RELEASE(pAdapterOutput);
	DX_SAFE_RELEASE(pAdapter);
	DX_SAFE_RELEASE(pFactory);


	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* pBackBuffer;
	
	swapChainDesc.BufferDesc.Width = m_width;
	swapChainDesc.BufferDesc.Height = m_height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = m_numSamples;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, DEVICE_CREATE_FLAG, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pContext);
	if (FAILED(res)) {
		MessageBoxA(0, "Could not create device", "Error", MB_OK);
		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, DEVICE_CREATE_FLAG, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pContext);
	}
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer, 0, &m_pRTView);

	DX_SAFE_RELEASE(pBackBuffer);


	// Create depth buffer
	D3D11_TEXTURE2D_DESC dsTexDesc;
	ID3D11Texture2D* pDSTexture;

	dsTexDesc.Width = m_width;
	dsTexDesc.Height = m_height;
	dsTexDesc.MipLevels = 1;
	dsTexDesc.ArraySize = 1;
	dsTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsTexDesc.SampleDesc.Count = m_numSamples;
	dsTexDesc.SampleDesc.Quality = 0;
	dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTexDesc.CPUAccessFlags = 0;
	dsTexDesc.MiscFlags = 0;

	m_pDevice->CreateTexture2D(&dsTexDesc, 0, &pDSTexture);
	m_pDevice->CreateDepthStencilView(pDSTexture, nullptr, &m_pDSView);

	DX_SAFE_RELEASE(pDSTexture);


	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthClipEnable = true;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.AntialiasedLineEnable = false;

	m_pDevice->CreateRasterizerState(&rsDesc, &m_pRSState);


	D3D11_BLEND_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BLEND_DESC));
	bDesc.AlphaToCoverageEnable = true;
	bDesc.IndependentBlendEnable = false;
	bDesc.RenderTarget[0].BlendEnable = true;
	bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_pDevice->CreateBlendState(&bDesc, &m_pBlendState);


	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = false;
	dsDesc.StencilEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState);
	
	
	//m_pSwapChain->SetFullscreenState(true, nullptr);

#if !defined(_DEBUG) && !defined(_WINDLL)
	m_pSwapChain->SetFullscreenState(true, nullptr);
#endif 
}

void CGraphics::Release() {
	DX_SAFE_RELEASE(m_pBlendState);
	DX_SAFE_RELEASE(m_pDSView);
	DX_SAFE_RELEASE(m_pRTView);
	DX_SAFE_RELEASE(m_pContext);
	DX_SAFE_RELEASE(m_pDevice);
	DX_SAFE_RELEASE(m_pSwapChain);
}

void CGraphics::SetBackBuffer() {
	m_pContext->OMSetRenderTargets(1, &m_pRTView, m_pDSView);
	SetViewport(0, 0, m_width, m_height);
}

void CGraphics::ClearBackBuffer(const Vector4& color, float depth) {
	m_pContext->ClearRenderTargetView(m_pRTView, color.v);
	m_pContext->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH, depth, 0);
}

void CGraphics::SwapBuffers() {
	m_pSwapChain->Present(1, 0);
}

void CGraphics::SetViewport(int x, int y, int width, int height) {
	D3D11_VIEWPORT vp;
	vp.TopLeftX = x;
	vp.TopLeftY = y;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pContext->RSSetViewports(1, &vp);
}

void CGraphics::SetDefaultRSState() {
	m_pContext->RSSetState(m_pRSState);
}

void CGraphics::EnableBlending() {
	if (!m_bBlendingEnabled) {
		Vector4 blendFactor(0.0f);
		m_pContext->OMSetBlendState(m_pBlendState, blendFactor.v, 0xFFFFFFFF);
		m_bBlendingEnabled = true;
	}
}

void CGraphics::DisableBlending() {
	if (m_bBlendingEnabled) {
		Vector4 blendFactor(0.0f);
		m_pContext->OMSetBlendState(nullptr, blendFactor.v, 0xFFFFFFFF);
		m_bBlendingEnabled = false;
	}
}