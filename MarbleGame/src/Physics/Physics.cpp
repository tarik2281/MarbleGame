//
//  Physics.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 03.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//


#include <functional>
#include <string>
#include <iterator>

#include "Physics.h"
#include "PhysicsDebugDrawer.h"
#include "EventManager/EventManager.h"
#include "EventManager/Events.h"
#include "Util.h"
#include "../main.h"

void PhysicsWrapper::Initialize() {
    m_interface = new btDbvtBroadphase();
    m_config = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_config);
    m_solver = new btSequentialImpulseConstraintSolver;
    //m_debugDrawer = new PhysicsDebugDrawer;
}

void PhysicsWrapper::Release() {
    delete m_solver;
    delete m_dispatcher;
    delete m_config;
    delete m_interface;
    //delete m_debugDrawer;
}

void PhysicsWrapper::Update() {
    
}

PhysicsWorld* PhysicsWrapper::GenerateNewWorld(float gravity) {
    PhysicsWorld* world = new btDiscreteDynamicsWorld(m_dispatcher, m_interface, m_solver, m_config);
    world->setGravity(btVector3(0.0f, gravity, 0.0f));
    //world->setDebugDrawer(m_debugDrawer);
	world->setInternalTickCallback(PhysicsWrapper::BulletInternalTickCallback);
    world->setWorldUserInfo((void*)this);
    return world;
}

PhysicsShape* PhysicsWrapper::MakeBoxShape(const Vector3 &halfSize) {
    btBoxShape* shape = new btBoxShape(halfSize);
    return shape;
}

PhysicsShape* PhysicsWrapper::MakeSphereShape(float radius) {
    btSphereShape* shape = new btSphereShape(radius);
    return shape;
}

PhysicsShape* PhysicsWrapper::MakeCylinderShape(float radius, float height) {
    btCylinderShape* shape = new btCylinderShape(Vector3(radius, height, radius));
    return shape;
}

PhysicsShape* PhysicsWrapper::MakeConvexShape(const Vector3 *vertices, int size) {
    btConvexHullShape* shape = new btConvexHullShape;
    for (int i = 0; i < size; i++)
        shape->addPoint(vertices[i]);
    return shape;
}

PhysicsBody* PhysicsWrapper::MakeBody(PhysicsShape* shape, const Vector3 &position, float mass) {
    btMotionState* state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));
    btVector3 fallInertia;
    shape->calculateLocalInertia(mass, fallInertia);
    PhysicsBody::btRigidBodyConstructionInfo ci(mass, state, shape, fallInertia);
    PhysicsBody* body = new PhysicsBody(ci);
    return body;
}

PhysicsBody* PhysicsWrapper::MakeBody(PhysicsShape* shape, PhysicsMotionState* state, float mass) {
    btVector3 fallInertia;
    shape->calculateLocalInertia(mass, fallInertia);
    PhysicsBody::btRigidBodyConstructionInfo ci(mass, state, shape, fallInertia);
    PhysicsBody* body = new PhysicsBody(ci);
    return body;
}

PhysicsBody* PhysicsWrapper::MakeStaticBody(PhysicsShape* shape, const Vector3& position, const Quaternion& orientation) {
    btMotionState* state = new btDefaultMotionState(btTransform(orientation, position));
    PhysicsBody::btRigidBodyConstructionInfo ci(0.0f, state, shape, btVector3(0, 0, 0));
    PhysicsBody* body = new PhysicsBody(ci);
    return body;
}

void PhysicsWrapper::DebugDraw() {

}

void PhysicsWrapper::BulletInternalTickCallback(btDynamicsWorld *world, btScalar timeStep) {
	PhysicsWrapper* physics = static_cast<PhysicsWrapper*>(world->getWorldUserInfo());
    CollisionList currentCollisions;
    
    int numManifolds = world->getDispatcher()->getNumManifolds();
    
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const PhysicsBody* obA = static_cast<const PhysicsBody*>(contactManifold->getBody0());
		const PhysicsBody* obB = static_cast<const PhysicsBody*>(contactManifold->getBody1());
        
        bool swapped = obA > obB;
        
        const PhysicsBody* sortedObjA = swapped ? obB : obA;
        const PhysicsBody* sortedObjB = swapped ? obA : obB;
        
		int numContacts = contactManifold->getNumContacts();
        if (numContacts >= 1) {
            CollisionPair pair(sortedObjA, sortedObjB);
            currentCollisions.insert(pair);
            CollisionList::iterator it = physics->m_collisionPairs.find(pair);
            if (it == physics->m_collisionPairs.end()) {
                GAME_FUNC(g_pEventManager->QueueEvent(EventDataPtr(DBG_NEW CollisionEvent(true, sortedObjA, sortedObjB))))
                printf("Collision added for b1=0x%x, b2 = 0x%x\n", sortedObjA, sortedObjB);
            }
        }
	}
    
    CollisionList removedCollisions;
    
    std::set_difference(physics->m_collisionPairs.begin(), physics->m_collisionPairs.end(), currentCollisions.begin(), currentCollisions.end(), std::inserter(removedCollisions, removedCollisions.begin()));
    
    CollisionList::iterator rIt;
    for (rIt = removedCollisions.begin(); rIt != removedCollisions.end(); ++rIt) {
        GAME_FUNC(g_pEventManager->QueueEvent(EventDataPtr(DBG_NEW CollisionEvent(false, rIt->first, rIt->second))))
        printf("Collision removed for b1=0x%x, b2 = 0x%x\n", rIt->first, rIt->second);
    }
    
    physics->m_collisionPairs = currentCollisions;
}