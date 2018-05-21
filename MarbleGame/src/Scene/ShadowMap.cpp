
#include "ShadowMap.h"

#include "Scene.h"
#include "Camera.h"
#include "CGraphics.h"
#include "Maths/GeometryMath.h"
#include "main.h"

#include "Resources/ResourceManager.h"

void ShadowMap::Initialize(int resolution, const Vector3& lightDir) {
	GraphicsDevice* pDevice = g_pGraphics->GetDevice();

	float s = 53.0f;
	m_projectionMatrix = MatrixMakeOrtho(-s, s, -s, s, 0.0f, s * 3.5f);
	m_viewMatrix = MatrixMakeLookAt(lightDir * s * 2.5f, VECTOR3_ZERO, VECTOR3_UP);

	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srViewDesc;
	ID3D11Texture2D* depthTexture;

	texDesc.Width = resolution;
	texDesc.Height = resolution;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.MipLevels = 1;

	HRESULT res = pDevice->CreateTexture2D(&texDesc, NULL, &depthTexture);

	ZeroMemory(&dsViewDesc, sizeof(dsViewDesc));
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Texture2D.MipSlice = 0;

	ZeroMemory(&srViewDesc, sizeof(srViewDesc));
	srViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srViewDesc.Texture2D.MipLevels = 1;

	res = pDevice->CreateDepthStencilView(depthTexture, &dsViewDesc, &m_pDepthView);
	res = pDevice->CreateShaderResourceView(depthTexture, &srViewDesc, &m_pResource);

	depthTexture->Release();

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.DepthBias = 100;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = true;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.MultisampleEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;

	pDevice->CreateRasterizerState(&rsDesc, &m_pRSState);
}

void ShadowMap::Begin() {
	GraphicsContext* pContext = g_pGraphics->GetContext();
	g_pGraphics->SetViewport(0, 0, SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
	pContext->OMSetRenderTargets(0, nullptr, m_pDepthView);
	pContext->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	pContext->RSSetState(m_pRSState);
}

void ShadowMap::OnRelease() {
	DX_SAFE_RELEASE(m_pRSState);
	DX_SAFE_RELEASE(m_pDepthView);
	DX_SAFE_RELEASE(m_pResource);
}


void CascadedShadowMap::Initialize() {
	TEMP_DEVICE(pDevice);

	ID3D11Texture2D* pShadowTexture;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srViewDesc;

	D3D11_TEXTURE2D_DESC shadowDesc;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.Width = SHADOWMAP_RESOLUTION * NUM_CASCADES;
	shadowDesc.Height = SHADOWMAP_RESOLUTION;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	pDevice->CreateTexture2D(&shadowDesc, nullptr, &pShadowTexture);

	ZeroMemory(&dsViewDesc, sizeof(dsViewDesc));
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Texture2D.MipSlice = 0;

	ZeroMemory(&srViewDesc, sizeof(srViewDesc));
	srViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srViewDesc.Texture2D.MipLevels = 1;

	pDevice->CreateDepthStencilView(pShadowTexture, &dsViewDesc, &m_pDepthView);
	pDevice->CreateShaderResourceView(pShadowTexture, &srViewDesc, &m_pResource);

	DX_SAFE_RELEASE(pShadowTexture);

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = true;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.MultisampleEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 1.0f;

	pDevice->CreateRasterizerState(&rsDesc, &m_pRSState);

	ZeroMemory(m_iCascadePartitions, NUM_MAX_CASCADES * sizeof(int));

	std::string path = ResourceManager::GetResourcesPath() + "CascadePartitions.txt";
	FILE* partitionsFile = fopen(path.c_str(), "r");
	fscanf(partitionsFile, "%d\n", m_iCascadePartitions);
	fscanf(partitionsFile, "%d\n", m_iCascadePartitions + 1);
	fscanf(partitionsFile, "%d\n", m_iCascadePartitions + 2);
	fscanf(partitionsFile, "%d\n", m_iCascadePartitions + 3);
	fclose(partitionsFile);

	switch (NUM_CASCADES) {
	case 1:
		m_vComparisonMask = Vector4(1, 0, 0, 0);
		break;
	case 2:
		m_vComparisonMask = Vector4(1, 1, 0, 0);
		break;
	case 3:
		m_vComparisonMask = Vector4(1, 1, 1, 0);
		break;
	case 4:
		m_vComparisonMask = Vector4(1, 1, 1, 1);
		break;
	}

	m_iMaxCascadePartition = 100;

	for (int i = 0; i < NUM_CASCADES; i++) {
		m_cascadeViewports[i].TopLeftX = i * SHADOWMAP_RESOLUTION;
		m_cascadeViewports[i].TopLeftY = 0;
		m_cascadeViewports[i].Width = SHADOWMAP_RESOLUTION;
		m_cascadeViewports[i].Height = SHADOWMAP_RESOLUTION;
		m_cascadeViewports[i].MinDepth = 0;
		m_cascadeViewports[i].MaxDepth = 1;
	}
}

void CascadedShadowMap::Begin() {
	m_fCascadePartitionsFrustum = Vector4(0.0f);

	const Matrix4& projMatrix = m_pScene->GetCamera()->getProjection();
	const Matrix4& viewMatrix = m_pScene->GetCamera()->getView();
	
	Matrix4 invViewMatrix = MatrixInvert(viewMatrix);

	float fFrustumIntervalBegin, fFrustumIntervalEnd;
	Vector4 vLightOrthoMin;
	Vector4 vLightOrthoMax;
	float fNearFarRange = 300.0f - 0.0f;

	Vector4 sceneAABBLightSpace[8];
	CornerPointsFromAABB(m_pScene->GetAABBMin(), m_pScene->GetAABBMax(), sceneAABBLightSpace);
	for (int i = 0; i < 8; i++) {
		sceneAABBLightSpace[i] = m_lightMatrix * sceneAABBLightSpace[i];
	}

	Frustum camFrustum = FrustumFromMatrix(projMatrix);

	for (int i = 0; i < NUM_CASCADES; i++) {
		fFrustumIntervalBegin = 0.0;
		fFrustumIntervalEnd = (float)m_iCascadePartitions[i];
		fFrustumIntervalBegin /= (float)m_iMaxCascadePartition;
		fFrustumIntervalEnd /= (float)m_iMaxCascadePartition;
		fFrustumIntervalBegin *= fNearFarRange;
		fFrustumIntervalEnd *= fNearFarRange;

		Vector4 frustumCorners[8];
		CornerPointsFromFrustum(camFrustum, frustumCorners, fFrustumIntervalBegin, fFrustumIntervalEnd);

		vLightOrthoMin = Vector4(FLT_MAX);
		vLightOrthoMax = Vector4(-FLT_MAX);

		for (int iCornerIndex = 0; iCornerIndex < 8; iCornerIndex++) {
			frustumCorners[iCornerIndex] = invViewMatrix * frustumCorners[iCornerIndex];

			Vector4 temp = m_lightMatrix * frustumCorners[iCornerIndex];
			vLightOrthoMin = Vector4Min(temp, vLightOrthoMin);
			vLightOrthoMax = Vector4Max(temp, vLightOrthoMax);
		}


		Vector4 vDiagonal = frustumCorners[0] - frustumCorners[6];
		vDiagonal = Vector4(vDiagonal.xyz().Length(), vDiagonal.w);

		float fCascadeBound = vDiagonal.x;

		Vector4 boarderOffset = (vDiagonal - (vLightOrthoMax - vLightOrthoMin)) * 0.5f;
		boarderOffset.z = boarderOffset.w = 0.0f;

		vLightOrthoMin -= boarderOffset;
		vLightOrthoMax += boarderOffset;
		vLightOrthoMin.x -= 1.0f;
		vLightOrthoMax.x += 1.0f;

		float fUnitsPerPixel = fCascadeBound / (float)SHADOWMAP_RESOLUTION;
		Vector4 vUnitsPerPixel(fUnitsPerPixel, fUnitsPerPixel, 1.0f, 1.0f);

		vLightOrthoMin /= vUnitsPerPixel;
		vLightOrthoMin.x = floorf(vLightOrthoMin.x);
		vLightOrthoMin.y = floorf(vLightOrthoMin.y);
		vLightOrthoMin.z = floorf(vLightOrthoMin.z);
		vLightOrthoMin.w = floorf(vLightOrthoMin.w);
		vLightOrthoMin *= vUnitsPerPixel;

		vLightOrthoMax /= vUnitsPerPixel;
		vLightOrthoMax.x = floorf(vLightOrthoMax.x);
		vLightOrthoMax.y = floorf(vLightOrthoMax.y);
		vLightOrthoMax.z = floorf(vLightOrthoMax.z);
		vLightOrthoMax.w = floorf(vLightOrthoMax.w);
		vLightOrthoMax *= vUnitsPerPixel;


		Vector4 lspMin = Vector4(FLT_MAX);
		Vector4 lspMax = Vector4(-FLT_MAX);

		for (int j = 0; j < 8; j++) {
			lspMin = lspMin.Min(sceneAABBLightSpace[j]);
			lspMax = lspMax.Max(sceneAABBLightSpace[j]);
		}


		float fNearPlane = fabsf(lspMax.z) / 2.0f;
		float fFarPlane = fabsf(lspMin.z);

		m_pCascadeMatrices[i] = MatrixMakeOrtho(vLightOrthoMin.x, vLightOrthoMax.x,
			vLightOrthoMin.y, vLightOrthoMax.y,
			fNearPlane, fFarPlane);
		m_fCascadePartitionsFrustum.v[i] = fFrustumIntervalEnd;
	}
}

void CascadedShadowMap::Render() {
	TEMP_CONTEXT(pContext);

	pContext->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11RenderTargetView* pNullView = nullptr;
	pContext->OMSetRenderTargets(1, &pNullView, m_pDepthView);
	pContext->RSSetState(m_pRSState);

	for (int i = 0; i < NUM_CASCADES; i++) {
		pContext->RSSetViewports(1, &m_cascadeViewports[i]);

		m_pScene->GetRenderData()->iShadowPass = i + 1;
		m_pScene->UpdateRenderData();

		m_pScene->OnRender();
	}

	g_pGraphics->SetDefaultRSState();
}

void CascadedShadowMap::Bind() {

}

void CascadedShadowMap::Release() {
	DX_SAFE_RELEASE(m_pResource);
	DX_SAFE_RELEASE(m_pDepthView);
	DX_SAFE_RELEASE(m_pRSState);
}