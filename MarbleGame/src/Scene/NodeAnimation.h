
#ifndef _NODE_ANIMATION_H_
#define _NODE_ANIMATION_H_

#include "SceneNode.h"

class INodeAnimation;

typedef std::shared_ptr<INodeAnimation> NodeAnimationPtr;
typedef std::weak_ptr<INodeAnimation> NodeAnimationRef;

class INodeAnimation
{
public:
	virtual void Start() = 0;
	virtual void Step() = 0;
	bool HasEnded() const {	return m_ended || m_pNode.expired(); }
	bool ShouldRemove() const { return m_remove; }
	SceneNodePtr GetNode() { return m_pNode.lock(); }

	void SetRemove(bool remove) { m_remove = remove; }

	INodeAnimation(SceneNodeRef node) {
		m_pNode = node;
		m_ended = false;
		m_remove = false;
	}
	virtual ~INodeAnimation() { }
protected:
	SceneNodeRef m_pNode;
	bool m_ended;
	bool m_remove;
};

class ScaleNodeAnimation : public INodeAnimation {
public:
	void Start() {
		GetNode()->SetScale(Vector3(m_from));
	}

	void Step() {
		SceneNodePtr node = GetNode();
		Vector3& lastScale = node->GetScale();
		float lastOpacity = node->GetOpacity();
		float speed = (m_to - m_from) / m_duration / 60.0f;
		Vector3 newScale = lastScale + speed;
		node->SetScale(newScale);
		node->SetOpacity(lastOpacity - 1.0f / m_duration / 60.0f);

		if (m_from > m_to)
			m_ended = newScale.x <= m_to;
		else
			m_ended = newScale.x >= m_to;

		if (m_ended)
			node->SetScale(Vector3(m_to));
	}

	ScaleNodeAnimation(SceneNodeRef node, float duration, float from, float to) : INodeAnimation(node),
		m_duration(duration), m_from(from), m_to(to) { }
private:
	float m_duration;
	float m_from;
	float m_to;
};

#endif // _NODE_ANIMATION_H_
