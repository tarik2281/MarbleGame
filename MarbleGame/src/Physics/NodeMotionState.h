#ifndef _NODE_MOTION_STATE_H_
#define _NODE_MOTION_STATE_H_

#include "Physics.h"
#include "Maths/VectorMath.h"
#include "Maths/QuaternionMath.h"

class NodeMotionState : public btMotionState {
public:
	NodeMotionState(SceneNode* node, const Vector3& pos) :
		m_pNode(node), m_position(pos) { }
	NodeMotionState(SceneNode* node, const Vector3& pos, const Quaternion& orient) :
		m_pNode(node), m_position(pos), m_orientation(orient) { }

	void getWorldTransform(btTransform& transform) const {
		transform.setOrigin(m_position);
		transform.setRotation(m_orientation);
	}

	void setWorldTransform(const btTransform& transform) {
		m_pNode->SetOrigin(transform.getOrigin());
		m_pNode->SetOrientation(transform.getRotation());
	}

private:
	SceneNode* m_pNode;
	Vector3 m_position;
	Quaternion m_orientation;
};

#endif // _NODE_MOTION_STATE_H_