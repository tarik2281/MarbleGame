
#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include "Maths/MatrixMath.h"
#include "Maths/VectorMath.h"
#include "CGraphics.h"

class ID3D11ShaderResourceView;
class ID3D11DepthStencilView;
class ID3D11RasterizerState;
class Scene;

class ShadowMap
{
public:
	Matrix4& GetProjection() { return m_projectionMatrix; }
	Matrix4& GetView() { return m_viewMatrix; }
	ID3D11ShaderResourceView* GetShaderResource() { return m_pResource; }
	ID3D11DepthStencilView* GetDepthView() { return m_pDepthView; }

	void Initialize(int, const Vector3&);
	void Begin();

	ShadowMap() {
		m_projectionMatrix = MatrixIdentity();
		m_viewMatrix = MatrixIdentity();
		m_pResource = nullptr;
		m_pDepthView = nullptr;
		m_pRSState = nullptr;
	}
	~ShadowMap() { OnRelease(); }
private:
	void OnRelease();

	Matrix4 m_projectionMatrix;
	Matrix4 m_viewMatrix;

	ID3D11ShaderResourceView* m_pResource;
	ID3D11DepthStencilView* m_pDepthView;
	ID3D11RasterizerState* m_pRSState;

	Scene* m_pScene;
};

#define NUM_MAX_CASCADES 4

class CascadedShadowMap {
public:
	void Initialize();
	void Begin();
	void Render();
	void Bind();
	void Release();

	CascadedShadowMap() { }
	~CascadedShadowMap() { }
public:
	Matrix4 m_lightMatrix;
	D3D11_VIEWPORT m_cascadeViewports[NUM_MAX_CASCADES];
	Matrix4 m_pCascadeMatrices[NUM_MAX_CASCADES];
	int m_iCascadePartitions[NUM_MAX_CASCADES];
	int m_iMaxCascadePartition;
	Vector4 m_fCascadePartitionsFrustum;
	Vector4 m_vComparisonMask;

	ID3D11ShaderResourceView* m_pResource;
	ID3D11DepthStencilView* m_pDepthView;
	ID3D11RasterizerState* m_pRSState;

	Scene* m_pScene;
};

#endif // _SHADOW_MAP_H_
