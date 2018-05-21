
#include <assert.h>

#include "Scene.h"
#include "Camera.h"
#include "ShadowMap.h"
#include "DXShader.h"
#include "CGraphics.h"
#include "main.h"

#define BUFFER_SLOT_BEGIN 0
#define BUFFER_SLOT_SCENE 0
#define BUFFER_SLOT_RENDER 1
#define BUFFER_SLOT_OBJECT 2

#define TEXTURE_SLOT_SHADOWMAP 6

bool Scene::AddNode(const SceneNodePtr& node) {
	assert(node);

	InstanceKey instanceId = node->GetInstanceId();
	if (instanceId != NODE_INSTANCE_NONE) {
		InstancingMap::iterator it = m_instances.find(instanceId);
		if (it == m_instances.end())
			return false;
		//assert(it != m_instances.end());

		InstancingNodePtr& instance = m_instances[instanceId];
		instance->AddNode(node);
		return true;
	}

	m_nodes.insert(node);
	m_vAABBMin = m_vAABBMin.Min(node->GetAABBMin() + node->GetOrigin());
	m_vAABBMax = m_vAABBMax.Max(node->GetAABBMax() + node->GetOrigin());
	node->SetScene(this);
	node->OnAdded();
	return true;
}

void Scene::RemoveNode(const SceneNodePtr& node) {
	assert(node);

	InstanceKey instanceId = node->GetInstanceId();
	if (instanceId != NODE_INSTANCE_NONE) {
		InstancingNodePtr& instance = m_instances[instanceId];
		instance->RemoveNode(node);
		return;
	}

	node->OnRemoved();
	node->SetScene(nullptr);
	m_nodes.erase(node);
}

void Scene::AddInstancingNode(const InstancingNodePtr& instance) {
	assert(instance);

	instance->SetScene(this);

	InstanceKey instanceId = instance->GetInstanceId();
	assert(instanceId != NODE_INSTANCE_NONE);
	
	m_instances.insert(InstancingPair(instanceId, instance));
	m_nodes.insert(SceneNodePtr(instance));
	instance->OnAdded();
}

void Scene::RemoveInstancingNode(const InstancingNodePtr& instance) {
	assert(instance);

	instance->SetScene(nullptr);

	InstanceKey instanceId = instance->GetInstanceId();
	assert(instanceId != NODE_INSTANCE_NONE);

	m_instances.erase(instanceId);
	m_nodes.erase(SceneNodePtr(instance));
	instance->OnRemoved();
}

void Scene::UpdateRenderData() {
	TEMP_CONTEXT(pContext);

	if (m_pcbRenderData)
		UpdateBufferStruct(pContext, m_pcbRenderData, m_renderData);
}

void Scene::UpdateObjectData() {
	TEMP_CONTEXT(pContext);

	if (m_pcbObjectData)
		UpdateBufferStruct(pContext, m_pcbObjectData, m_objectData);
}

void Scene::UpdateAABB() {
	m_vAABBMin = Vector3(FLT_MAX);
	m_vAABBMax = Vector3(-FLT_MAX);

	ONodeSet::iterator it;
	for (it = m_nodes.begin(); it != m_nodes.end(); ++it) {
		m_vAABBMin = m_vAABBMin.Min((*it)->GetAABBMin() + (*it)->GetOrigin());
		m_vAABBMax = m_vAABBMax.Max((*it)->GetAABBMax() + (*it)->GetOrigin());
	}
}

void Scene::RunAnimation(const NodeAnimationPtr& anim) {
	assert(anim);

	anim->Start();
	m_animations.push_back(anim);
}

void Scene::Initialize(const Vector3& lightDir) {
	TEMP_DEVICE(pDevice);

	memset(&m_sceneData, 0, sizeof(SceneData));
	memset(&m_renderData, 0, sizeof(RenderData));
	memset(&m_objectData, 0, sizeof(ObjectData));
	
	MakeDynamicBuffer(pDevice, m_pcbSceneData, D3D11_BIND_CONSTANT_BUFFER, sizeof(SceneData));
	MakeDynamicBuffer(pDevice, m_pcbRenderData, D3D11_BIND_CONSTANT_BUFFER, sizeof(RenderData));
	MakeDynamicBuffer(pDevice, m_pcbObjectData, D3D11_BIND_CONSTANT_BUFFER, sizeof(ObjectData));

	m_effectManager.Initialize();

	m_lightDirection = lightDir;

	m_shadowMap.m_lightMatrix = MatrixMakeLookAt(Vector3(-20.0f, 10.0f, -5.0f), VECTOR3_ZERO, VECTOR3_UP);
	float s = 53.0f;
	m_shadowMap.m_lightMatrix = MatrixMakeLookAt(lightDir * 300.0f, VECTOR3_ZERO, VECTOR3_UP);
	m_shadowMap.m_pScene = this;
	m_shadowMap.Initialize();
}

void Scene::UpdateData() {
	InstancingMap::iterator it;
	for (it = m_instances.begin(); it != m_instances.end(); ++it)
		it->second->UpdateData();
}

void Scene::UpdateAnimations() {
	AnimationList::iterator it = m_animations.begin();
	AnimationList::iterator end = m_animations.end();

	while (it != end) {
		INodeAnimation* anim = it->get();

		if (anim->HasEnded()) {
			if (anim->ShouldRemove())
				RemoveNode(anim->GetNode());

			m_animations.erase(it++);
		}
		else {
			anim->Step();
			++it;
		}
	}
}

void Scene::Begin() {
	TEMP_CONTEXT(pContext);

	UpdateAABB();

	ID3D11ShaderResourceView* unbind = nullptr;
	pContext->PSSetShaderResources(TEXTURE_SLOT_SHADOWMAP, 1, &unbind);

	if (m_iCurrentCam == 0) {
		m_sceneData.mProjection = /*m_shadowMap.m_pCascadeMatrices[0].Transpose();*/ m_pCamera->getProjection().Transpose();
		m_sceneData.mView = /*m_shadowMap.m_lightMatrix.Transpose();*/ m_pCamera->getView().Transpose();
	}
	else {
		m_sceneData.mProjection = m_shadowMap.m_pCascadeMatrices[m_iCurrentCam - 1].Transpose();
		m_sceneData.mView = m_shadowMap.m_lightMatrix.Transpose();
	}

	m_sceneData.mViewProjection = m_sceneData.mView * m_sceneData.mProjection;
	m_sceneData.vCameraPosition = Vector4(m_pCamera->getPosition(), 1.0f);

	m_sceneData.mShadowView = m_shadowMap.m_lightMatrix.Transpose();
	for (int i = 0; i < NUM_CASCADES; i++) {
		m_sceneData.mCascades[i] = m_shadowMap.m_pCascadeMatrices[i].Transpose();
	}
	m_sceneData.fCascadePartitionsFrustum = m_shadowMap.m_fCascadePartitionsFrustum;
	m_sceneData.fElapsedTime += g_fFrameRate;
	m_sceneData.vShadowResolution = Vector2(SHADOWMAP_RESOLUTION * NUM_CASCADES, SHADOWMAP_RESOLUTION);
	m_sceneData.vComparisonMask = m_shadowMap.m_vComparisonMask;
	UpdateBufferStruct(pContext, m_pcbSceneData, m_sceneData);

	m_renderData.iShadowPass = 0;
	UpdateBufferStruct(pContext, m_pcbRenderData, m_renderData);

	ID3D11Buffer* buffers[2] = {
		m_pcbSceneData, m_pcbRenderData
	};

	pContext->VSSetConstantBuffers(BUFFER_SLOT_BEGIN, 2, buffers);
	pContext->PSSetConstantBuffers(BUFFER_SLOT_BEGIN, 2, buffers);

	m_effectManager.SetIsShading(false);
	m_effectManager.Begin();

	m_shadowPass = true;
	m_shadowMap.Begin();
	m_shadowMap.Render();
	g_pGraphics->SetDefaultRSState();

	ID3D11ShaderResourceView* pView = m_shadowMap.m_pResource;
	pContext->OMSetRenderTargets(0, 0, 0);
	pContext->PSSetShaderResources(TEXTURE_SLOT_SHADOWMAP, 1, &pView);

	m_renderData.iShadowPass = 0;
	UpdateBufferStruct(pContext, m_pcbRenderData, m_renderData);
	pContext->VSSetConstantBuffers(BUFFER_SLOT_RENDER, 1, &m_pcbRenderData);
	pContext->PSSetConstantBuffers(BUFFER_SLOT_RENDER, 1, &m_pcbRenderData);
	m_effectManager.SetIsShading(true);
}

void Scene::Draw(ID3D11RenderTargetView* rtView, ID3D11DepthStencilView* dsView) {
	g_pGraphics->SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (rtView != nullptr && dsView != nullptr)
		g_pGraphics->GetContext()->OMSetRenderTargets(1, &rtView, dsView);
	
	m_shadowPass = false;
	OnRender();
}

void Scene::OnRelease() {
	m_shadowMap.Release();
	DX_SAFE_RELEASE(m_pcbSceneData);
	DX_SAFE_RELEASE(m_pcbRenderData);
	DX_SAFE_RELEASE(m_pcbObjectData);
}

void Scene::OnRender() {
	ONodeSet::iterator it;
	for (it = m_nodes.begin(); it != m_nodes.end(); ++it)
		RenderNode(*it);

	m_pCurrentNode = nullptr;
}

void Scene::RenderNode(const SceneNodePtr& node) {
	TEMP_CONTEXT(pContext);

	m_pCurrentNode = node.get();

	if (!m_shadowPass || node->HasShadows()) {
		m_objectData.mModel = node->GetRelTransform().Transpose();
		m_objectData.vColor = node->GetColor();
		UpdateBufferStruct(pContext, m_pcbObjectData, m_objectData);
		pContext->VSSetConstantBuffers(BUFFER_SLOT_OBJECT, 1, &m_pcbObjectData);
		pContext->PSSetConstantBuffers(BUFFER_SLOT_OBJECT, 1, &m_pcbObjectData);
		node->OnRender();
	}

	NodeSet::iterator it;
	for (it = node->m_subNodes.begin(); it != node->m_subNodes.end(); ++it)
		RenderNode(*it);
}

void Scene::NextCam() {
	m_iCurrentCam++;
	
	if (m_iCurrentCam > 3)
		m_iCurrentCam = 0;
}

void Scene::ToggleVisualCascades() {
	m_renderData.bVisualizeCascades = !m_renderData.bVisualizeCascades;
}