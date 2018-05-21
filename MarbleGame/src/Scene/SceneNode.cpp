
#include <assert.h>

#include "SceneNode.h"
#include "DXShader.h"
#include "Scene.h"
#include "Resources/Model.h"

SceneNode::SceneNode() {
	m_origin = VECTOR3_ZERO;
	m_orientation = QUATERNION_IDENTITY;
	m_scale = Vector3(1.0f);
	m_color = Vector4(1.0f);
	m_opacity = 1.0f;
	m_pScene = nullptr;
	m_pParent = nullptr;
	m_vAABBMin = Vector3(FLT_MAX);
	m_vAABBMax = Vector3(-FLT_MAX);
}

bool SceneNode::NodeSorter::operator()(const SceneNodePtr& first, const SceneNodePtr& second) {
	RenderPass fP = first->GetRenderPass();
	RenderPass sP = second->GetRenderPass();

	if (fP == RenderPassPre)
		return (sP != RenderPassPre) || ((sP == RenderPassPre) && first < second);
	if (sP == RenderPassPre)
		return false;

	if (fP == RenderPassPost)
		return ((sP == RenderPassPost) && first < second);
	if (sP == RenderPassPost)
		return true;

	return first < second;
}

void SceneNode::AddNode(const SceneNodePtr& node) {
	assert(node);

	node->SetScene(m_pScene);
	node->SetParent(this);
	node->OnAdded();
	m_subNodes.insert(node);
}

void SceneNode::RemoveNode(const SceneNodePtr& node) {
	assert(node);

	node->SetScene(nullptr);
	node->SetParent(nullptr);
	node->OnRemoved();
	m_subNodes.erase(node);
}

Matrix4 SceneNode::GetAbsTransform() const {
	Matrix4 transform = GetRelTransform();

	if (m_pParent)
		return m_pParent->GetAbsTransform() * transform;

	return transform;
}

Matrix4 SceneNode::GetRelTransform() const {
	Matrix4 transform = MatrixTranslate(m_origin) * MatrixScale(m_scale) * m_orientation.ToMatrix();
	return transform;
}

Vector3 SceneNode::GetAbsPosition() const {
	Vector3 origin = m_origin;

	if (m_pParent)
		return m_pParent->GetAbsPosition() + origin;

	return origin;
}


ModelNode::ModelNode(Model* model, Material mat) : SceneNode() {
	m_pModel = model;
	m_mat = mat;
	m_vAABBMin = model->GetMin();
	m_vAABBMax = model->GetMax();
}

void ModelNode::OnAdded() {
	m_pEffect = EffectManager->LoadEffect("Shader", TexCoord2D, true, true);
}

void ModelNode::OnRender() {
	TEMP_CONTEXT(pContext);

	if (m_mat.diffuseMap && m_mat.normalMap) {
		ID3D11ShaderResourceView* resources[2] = {
			m_mat.diffuseMap->m_pView, m_mat.normalMap->m_pView
		};
		pContext->PSSetShaderResources(0, 2, resources);
	}

	EffectManager->ApplyEffect(m_pEffect);
	m_pModel->Render();

}

SceneNode::RenderPass ModelNode::GetRenderPass() const {
	return (m_mat.opacity == 1.0f ? RenderPassStandard : RenderPassPost);
}