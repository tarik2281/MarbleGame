#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "Level.h"
#include "Scene/SceneNode.h"
#include "Scene/InstancingNode.h"
#include "StringHash.h"

#define PLATFORM_FLAG 'o'
#define RAMP_FLAG 'r'
#define CORNER_FLAG 'c'

#define FLAG_IS_PLATFORM(flag) (flag == PLATFORM_FLAG || flag == RAMP_FLAG || flag == CORNER_FLAG)

#define NUM_MAX_PLATFORM_VERTICES 24
#define NUM_MAX_PLATFORM_INDICES 36

enum PlatformRotation {
	North = 0,
	West,
	South,
	East
};

#ifndef _WINDLL
class PlatformInstances : public InstancingNode<PlatformNode*> {
public:
	PlatformInstances(Material mat);

	void InitData(int size = 0);

	void UpdateData() { }
	PlatformNode* GetDataFromNode(Node* node) { return (PlatformNode*)node; }

protected:
	InstanceKey GetInstanceId() const { return m_mat.identifier; }

	void OnAdded();
	void OnRender();
	void OnRelease();
	
	ID3D11Buffer* m_indexBuffer;
	unsigned int m_numIndices;
	Material m_mat;
	CBaseEffect* m_pEffect;
};
#endif

class PlatformData : public LevelObjectData {
public:

	char material[100];
	BoundingBox bb;
	PlatformRotation rotation;
	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;

	Vector3 GetPosition() const;
	Quaternion GetRotation() const {
		return Quaternion(VectorUp, rotation * 90.0f);
	}

	bool IsPlatform() const {
		return true;
	}

#ifdef _WINDLL
	virtual Vector3 GetPositionEditor() const {
		return bb.origin;
	}

	virtual void SetPosition(Vector3 position) {
		bb.origin = position;
	}

	virtual void SetRotation(Quaternion q) {
		rotation = (PlatformRotation)((int)q.y);
	}
#endif

	char getFlag() const {
		return PLATFORM_FLAG;
	}

	PlatformData() { }
	PlatformData(const BoundingBox& box, PlatformRotation pRotation = North) : bb(box), rotation(pRotation) { }
};

class RampData : public PlatformData {
public:
	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;

	char getFlag() const {
		return RAMP_FLAG;
	}

	RampData() { }
	RampData(const BoundingBox& box, PlatformRotation pRotation = North) : PlatformData(box, pRotation) { }
};

class CornerData : public PlatformData {
public:
	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;

	char getFlag() const {
		return CORNER_FLAG;
	}

	CornerData() { }
	CornerData(const BoundingBox& box, PlatformRotation pRotation = North) : PlatformData(box, pRotation) { }
};

#ifdef _WINDLL
#define NODE_BASE_CLASS SceneNode
#else
#define NODE_BASE_CLASS InstancedNode<PlatformNode*>
#endif

class PlatformNode : public NODE_BASE_CLASS {
public:
	PlatformNode() { }
	PlatformNode(Material mat, BoundingBox bb) : NODE_BASE_CLASS(), mat(mat), bb(bb) {
		Initialize();
	}

	void Initialize();
	void updateTransform();
	BoundingBox bb;
	void Resize();

	Material mat;
protected:
	friend class PlatformInstances;

	virtual void MakeVertices(Vertex*, uint16_t*);
	virtual int GetNumVertices() const { return NUM_MAX_PLATFORM_VERTICES; }
	virtual int GetNumIndices() const { return NUM_MAX_PLATFORM_INDICES; }

#ifndef _WINDLL
	InstanceKey GetInstanceId() const { return mat.identifier; }
#endif

	void OnRender();
	void OnRelease();

#ifdef _WINDLL
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_indicesSize;
#endif
};



class RampNode : public PlatformNode {
public:
	RampNode(Material pMat, BoundingBox pBB) : PlatformNode() { mat = pMat; bb = pBB; Initialize(); }
protected:
	void MakeVertices(Vertex*, uint16_t*);
	int GetNumVertices() const { return 18; }
	int GetNumIndices() const { return 24; }
};


class CornerNode : public PlatformNode {
public:
	CornerNode(Material pMat, BoundingBox pBB) : PlatformNode() { mat = pMat; bb = pBB; Initialize(); }
protected:
	void MakeVertices(Vertex*, uint16_t*);
	int GetNumVertices() const { return 18; }
	int GetNumIndices() const { return 24; }
};


inline void PlatformNode::updateTransform() {
	SetOrigin(bb.origin);
}

#endif