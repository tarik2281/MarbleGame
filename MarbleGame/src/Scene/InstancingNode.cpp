
#include "InstancingNode.h"
#include "Scene.h"
#include "DXShader.h"

StarInstances::StarInstances() : _Base() {
	rotStep = Quaternion(VectorUp, 2.0f);
}

void StarInstances::OnAdded() {
	m_pEffect = EffectManager->LoadEffect<CInstanceEffect>("StarShader", TexCoord2D, true, true);
}

void StarInstances::OnRender() {
	GraphicsContext* pContext = g_pGraphics->GetContext();

	Mesh* mesh = m_model->Begin();
	UINT stride = sizeof(_InstanceType);
	UINT offset = 0;
	pContext->IASetVertexBuffers(1, 1, &m_instanceBuffer, &stride, &offset);
	
	EffectManager->ApplyEffect(m_pEffect);
	pContext->DrawIndexedInstanced(mesh->indicesSize, m_instances.size(), 0, 0, 0);
}