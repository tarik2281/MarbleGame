#ifndef _SCENE_H_
#define _SCENE_H_

#include <map>

#include "SceneNode.h"
#include "NodeAnimation.h"
#include "InstancingNode.h"
#include "DXShader.h"
#include "ShadowMap.h"

class Camera;
class ShadowMap;

class ID3D11RenderTargetView;
class ID3D11DepthStencilView;

__declspec(align(16))
struct SceneData {
	Matrix4 mProjection;
	Matrix4 mView;
	Matrix4 mViewProjection;
	Vector4 vCameraPosition;

	Matrix4 mCascades[NUM_MAX_CASCADES];
	Matrix4 mShadowView;
	Vector4 fCascadePartitionsFrustum;
	int numCascades;
	float fElapsedTime;
	Vector2 vShadowResolution;
	Vector4 vComparisonMask;
};

__declspec(align(16))
struct ObjectData {
	Matrix4 mModel;
	Vector4 vColor;
};

__declspec(align(16))
struct RenderData {
	Vector4 vClipPlane;
	int iShadowPass;
	int bMirror;
	float fWaterHeight;
	int bVisualizeCascades;
};

class Scene {
public:
	bool AddNode(const SceneNodePtr&);
	void RemoveNode(const SceneNodePtr&);

	void AddInstancingNode(const InstancingNodePtr&);
	void RemoveInstancingNode(const InstancingNodePtr&);

	CEffectManager* GetEffectManager();
	RenderData* GetRenderData();
	ObjectData* GetObjectData();
	void UpdateRenderData();
	void UpdateObjectData();

	const Vector3& GetAABBMin() const;
	const Vector3& GetAABBMax() const;
	void UpdateAABB();

	void RunAnimation(const NodeAnimationPtr&);

	void Initialize(const Vector3&);
	void UpdateData();
	void UpdateAnimations();

	void Begin();
	void Draw(ID3D11RenderTargetView*, ID3D11DepthStencilView*);

	void SetCamera(Camera* cam) { m_pCamera = cam; }
	Camera* GetCamera() const { return m_pCamera; }
	void NextCam();
	void ToggleVisualCascades();

	Scene() {
		m_shadowPass = false;
		m_lightDirection = VectorZero;
		m_pCamera = nullptr;
		m_pCurrentNode = nullptr;
		m_pcbSceneData = nullptr;
		m_pcbRenderData = nullptr;
		m_pcbObjectData = nullptr;
		m_vAABBMin = Vector3(FLT_MAX);
		m_vAABBMax = Vector3(-FLT_MAX);
		m_iCurrentCam = 0;
	}
	~Scene() { OnRelease(); }
protected:
	void OnRelease();
	void OnRender();
private:
	friend class CascadedShadowMap;
	typedef std::pair<InstanceKey, InstancingNodePtr> InstancingPair;
	typedef std::map<InstanceKey, InstancingNodePtr> InstancingMap;
	typedef std::list<NodeAnimationPtr> AnimationList;

	void RenderNode(const SceneNodePtr&);

	bool m_shadowPass;
	Vector3 m_lightDirection;

	CEffectManager m_effectManager;
	CascadedShadowMap m_shadowMap;
	Camera* m_pCamera;
	SceneNode* m_pCurrentNode; // Rendering
	
	ONodeSet m_nodes;
	InstancingMap m_instances;
	AnimationList m_animations;
	Vector3 m_vAABBMin;
	Vector3 m_vAABBMax;

	SceneData m_sceneData;
	RenderData m_renderData;
	ObjectData m_objectData;
	ID3D11Buffer* m_pcbSceneData;
	ID3D11Buffer* m_pcbRenderData;
	ID3D11Buffer* m_pcbObjectData;

	int m_iCurrentCam;
};

inline CEffectManager* Scene::GetEffectManager() {
	return &m_effectManager;
}

inline RenderData* Scene::GetRenderData() {
	return &m_renderData;
}

inline ObjectData* Scene::GetObjectData() {
	return &m_objectData;
}

inline const Vector3& Scene::GetAABBMin() const {
	return m_vAABBMin;
}

inline const Vector3& Scene::GetAABBMax() const {
	return m_vAABBMax;
}

#endif // _SCENE_H_