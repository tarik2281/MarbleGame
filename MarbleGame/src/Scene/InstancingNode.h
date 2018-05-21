#ifndef _INSTANCING_NODE_H_
#define _INSTANCING_NODE_H_

#include <list>

#include "SceneNode.h"
#include "CGraphics.h"
#include "Resources/Model.h"


template<class T> class InstancedNode;

class IInstancingNode : public SceneNode {
public:
	virtual void InitData(int size = 0) = 0;
	virtual void UpdateData() = 0;
};

typedef std::shared_ptr<IInstancingNode> InstancingNodePtr;
#define MakeInstancingNodePtr(ptr) InstancingNodePtr(ptr, SceneNode::NodeDeleter())

template<class T>
class InstancingNode : public IInstancingNode {
protected:
	typedef typename InstancingNode<T> _Base;
	typedef T _InstanceType;
	typedef typename std::list<_InstanceType> InstanceList;
	typedef typename InstancedNode<_InstanceType> Node;

	InstanceList m_instances;
	ID3D11Buffer* m_instanceBuffer;
	bool m_static;
	bool m_changed;
	bool m_flexible;
	unsigned int m_maxSize;

	void OnRelease() {
		m_instanceBuffer->Release();
	}

public:
	void AddNode(const SceneNodePtr&);
	void RemoveNode(const SceneNodePtr&);

	void renderInstances(int indexCount) const {
		UINT stride = sizeof(_InstanceType);
		UINT offset = 0;
		g_pDevice->IASetVertexBuffers(1, 1, &m_instanceBuffer, &stride, &offset);
		g_pDevice->DrawIndexedInstanced(indexCount, m_instances.size(), 0, 0, 0);
	}

	void InitData(int size = 0) {
		if (size == 0)
			size = m_instances.size();
		if (size == 0)
			size = 1;
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = sizeof(_InstanceType) * size;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;

		g_pGraphics->GetDevice()->CreateBuffer(&desc, NULL, &m_instanceBuffer);
		UpdateData();
		m_changed = false;
		m_maxSize = size;
	}

	void UpdateData() {
		if (m_changed) {
			GraphicsContext* pContext = g_pGraphics->GetContext();

			D3D11_MAPPED_SUBRESOURCE data;
			_InstanceType* positions;
			pContext->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			positions = (_InstanceType*)data.pData;
			InstanceList::iterator it = m_instances.begin();
			int index = 0;
			for (; it != m_instances.end(); ++it) {
				positions[index] = *it;
				index++;
			}
			pContext->Unmap(m_instanceBuffer, 0);
			m_changed = false;
		}
	}

	virtual _InstanceType GetDataFromNode(Node*) = 0;
	void SetData(SceneNode* object) {
		Node* node = (Node*)object;
		*node->index = GetDataFromNode(node);
		m_changed = true;
	}

	InstancingNode<_InstanceType>() : IInstancingNode() {
		m_static = false;
		m_instanceBuffer = 0;
		m_changed = true;
#ifndef _WINDLL
		m_flexible = false;
#else 
		m_flexible = true;
#endif
	}
};

template<class T>
class InstancedNode : public SceneNode {
protected:
	typedef typename InstancedNode<T> _Base;
	typedef typename std::list<T>::iterator IteratorType;
	typedef typename InstancingNode<T> Node;
	friend class Node;

	IteratorType index;
	Node* m_instancingParent;
public:
	void OnRender() { }
	void OnRelease() { }

	void SetOrigin(const Vector3& pos) {
		SceneNode::SetOrigin(pos);
		if (m_instancingParent)
		m_instancingParent->SetData(this);
	}

	void SetScale(const Vector3& scale) {
		SceneNode::SetScale(scale);
		if (m_instancingParent)
		m_instancingParent->SetData(this);
	}

	InstancedNode<T>() : SceneNode() {
		m_instancingParent = nullptr;
	}
};


template<class T>
inline void InstancingNode<T>::AddNode(const SceneNodePtr& object) {
	Node* node = (Node*)object.get();
	node->m_instancingParent = this;
	T data = GetDataFromNode(node);
	m_instances.push_front(data);
	node->index = m_instances.begin();
	SceneNode::AddNode(object);

	m_changed = true;

	if (m_instanceBuffer && m_flexible) {
		if (m_instances.size() >= m_maxSize) {
			OutputDebugStringW(L"Reallocating instance buffer\n");
			m_instanceBuffer->Release();
			
			InitData(m_instances.size() + 10);
		}
	}
}

template<class T>
inline void InstancingNode<T>::RemoveNode(const SceneNodePtr& object) {
	Node* node = (Node*)object.get();
	m_instances.erase(node->index);
	SceneNode::RemoveNode(object);
	m_changed = true;
}

class StarInstances : public InstancingNode<Vector4> {
public:
	Quaternion rotStep;
	Model* m_model;
public:
	StarInstances();

	Vector4 GetDataFromNode(Node* node) {
		Vector3 orig = node->GetOrigin();
		Vector4 temp(orig.x, orig.y, orig.z, node->GetScale().x);
		return temp;
	}

	void UpdateData() {
		_Base::UpdateData();
		Quaternion rot = GetOrientation();
		SetOrientation(rot * rotStep);
	}
protected:
	void OnAdded();
	void OnRender();

	InstanceKey GetInstanceId() const { return 1; }

	CBaseEffect* m_pEffect;
};

class StarNode : public InstancedNode<Vector4> {
protected:
	InstanceKey GetInstanceId() const { return 1; }
};


#endif /* _INSTANCING_NODE_H_ */