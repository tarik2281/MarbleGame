//
//  Physics.h
//  MarbleGame
//
//  Created by Tarik Karaca on 03.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__Physics__
#define __MarbleGame__Physics__

#ifdef _WIN32
#include "btBulletDynamicsCommon.h"
#else
#include <BulletDynamics/btBulletDynamicsCommon.h>
#endif

#include <memory>
#include <map>
#include <set>

#include "Maths\VectorMath.h"
#include "Maths\QuaternionMath.h"

#ifdef _DEBUG
#define COLLISION_LIB "BulletCollision_D.lib"
#define DYNAMICS_LIB "BulletDynamics_D.lib"
#define LINEARMATH_LIB "LinearMath_D.lib"
#else
#define COLLISION_LIB "BulletCollision.lib"
#define DYNAMICS_LIB "BulletDynamics.lib"
#define LINEARMATH_LIB "LinearMath.lib"
#endif

#pragma comment(lib, COLLISION_LIB)
#pragma comment(lib, DYNAMICS_LIB)
#pragma comment(lib, LINEARMATH_LIB)

#define DEFAULT_GRAVITY -30.0f

typedef btCollisionShape PhysicsShape;
typedef btMotionState PhysicsMotionState;
typedef btDynamicsWorld PhysicsWorld;

class PhysicsDebugDrawer;

class PhysicsBody : public btRigidBody {
public:
    void Release();
    
    PhysicsBody(const btRigidBodyConstructionInfo& ci) : btRigidBody(ci) { }
	~PhysicsBody() { Release(); }
};

class PhysicsWrapper {
public:
    void Initialize();
    void Release();
    void Update();
    void DebugDraw();
    
    PhysicsWorld* GenerateNewWorld(float gravity = DEFAULT_GRAVITY);
    
    PhysicsBody* MakeBody(PhysicsShape*, const Vector3& position, float mass);
    PhysicsBody* MakeBody(PhysicsShape*, PhysicsMotionState*, float mass);
    PhysicsBody* MakeStaticBody(PhysicsShape*, const Vector3&, const Quaternion& = QUATERNION_IDENTITY);
    
    PhysicsShape* MakeBoxShape(const Vector3& halfSize);
    PhysicsShape* MakeSphereShape(float radius);
    PhysicsShape* MakeCylinderShape(float radius, float height);
    PhysicsShape* MakeConvexShape(const Vector3*, int);
    
    static void BulletInternalTickCallback(btDynamicsWorld* world, btScalar timeStep);
private:
    typedef std::pair<const PhysicsBody*, const PhysicsBody*> CollisionPair;
    typedef std::set<CollisionPair> CollisionList;
    
    CollisionList m_collisionPairs;
    
    btBroadphaseInterface*  m_interface;
    btDefaultCollisionConfiguration* m_config;
    btCollisionDispatcher* m_dispatcher;
    btSequentialImpulseConstraintSolver* m_solver;
    PhysicsDebugDrawer* m_debugDrawer;
};

inline void PhysicsBody::Release() {
	PhysicsShape* shape = getCollisionShape();
	void* user = shape->getUserPointer();
	if (user) {
		btTriangleMesh* mesh = (btTriangleMesh*)user;
		delete mesh;
	}
    delete shape;
    delete getMotionState();
}

#endif /* defined(__MarbleGame__Physics__) */
