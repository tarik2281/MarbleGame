#ifndef _SCENE_NODE_H_
#define _SCENE_NODE_H_

#include <memory>
#include <set>

#include "Material.h"
#include "Maths/VectorMath.h"
#include "Maths/QuaternionMath.h"
#include "Maths/MatrixMath.h"

class Scene;
class SceneNode;
class Model;
class CBaseEffect;

typedef std::shared_ptr<SceneNode> SceneNodePtr;
typedef std::weak_ptr<SceneNode> SceneNodeRef;
typedef std::set<SceneNodePtr> NodeSet;

#define NODE_INSTANCE_NONE 0

#define MakeNodePtr(ptr) SceneNodePtr(ptr, SceneNode::NodeDeleter())

#ifdef _WINDLL
#define EDITOR_FUNC(ex) ex
#else
#define EDITOR_FUNC(ex)
#endif

#define EffectManager m_pScene->GetEffectManager()

typedef uint64_t InstanceKey;

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
	struct NodeSorter {
		bool operator()(const SceneNodePtr& first, const SceneNodePtr& second);
	};

	struct NodeDeleter {
		void operator()(SceneNode* ptr) { ptr->OnRelease(); delete ptr; }
	};

	virtual void AddNode(const SceneNodePtr&);
	virtual void RemoveNode(const SceneNodePtr&);

	virtual void SetOrigin(const Vector3& v) { m_origin = v; }
	virtual void SetOrientation(const Quaternion& q) { m_orientation = q; }
	virtual void SetScale(const Vector3& v) { m_scale = v; }
	virtual void SetOpacity(float f) { m_opacity = f; }
	virtual void SetColor(const Vector4& v) { m_color = v; }

	virtual Vector3& GetOrigin() { return m_origin; }
	virtual const Vector3& GetOrigin() const { return m_origin; }
	virtual Quaternion& GetOrientation() { return m_orientation; }
	virtual const Quaternion& GetOrientation() const { return m_orientation; }
	virtual Vector3& GetScale() { return m_scale; }
	virtual const Vector3& GetScale() const { return m_scale; }
	virtual Vector4& GetColor() { return Vector4(m_color.r, m_color.g, m_color.b, m_opacity); }
	virtual float GetOpacity() const { return m_opacity; }

	Matrix4 GetAbsTransform() const;
	Matrix4 GetRelTransform() const;

	Vector3 GetAbsPosition() const;

	const Vector3& GetAABBMin() const { return m_vAABBMin; }
	const Vector3& GetAABBMax() const { return m_vAABBMax; }

	bool HasParent() const { return (m_pParent != nullptr); }

	SceneNode();
	~SceneNode() {  }
protected:
	enum RenderPass {
		RenderPassPre,
		RenderPassStandard,
		RenderPassPost
	};

	virtual RenderPass GetRenderPass() const { return RenderPassStandard; }
	virtual bool HasShadows() const { return true; }
	virtual InstanceKey GetInstanceId() const { return NODE_INSTANCE_NONE; }

	virtual void OnInitialize() { }
	virtual void OnAdded() { }
	virtual void OnRemoved() { }
	virtual void OnRelease() { }
	virtual void OnUpdateData() { }
	virtual void OnRender() = 0;

	void SetScene(Scene* scene) { m_pScene = scene; }
	void SetParent(SceneNode* parent) { m_pParent = parent; }

	Scene* m_pScene;

	NodeSet m_subNodes;
	Vector3 m_vAABBMin;
	Vector3 m_vAABBMax;
private:
	friend class Scene;

	Vector3 m_origin;
	Quaternion m_orientation;
	Vector3 m_scale;
	Vector4 m_color;
	float m_opacity;

	SceneNode* m_pParent;
};

typedef std::set<SceneNodePtr, SceneNode::NodeSorter> ONodeSet;


class ModelNode : public SceneNode {
public:
	ModelNode(Model*, Material);
protected:
	void OnAdded();
	void OnRender();
	void OnRelease() { }

	RenderPass GetRenderPass() const;

	Model* m_pModel;
	Material m_mat;
	CBaseEffect* m_pEffect;
};

#endif // _SCENE_NODE_H_