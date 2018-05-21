//
//  GameLogic.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 06.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "GameLogic.h"

#include "Util.h"
#include "Level.h"
#include "Platform.h"

#include "../main.h"

#define TIME_STEP (1.0f / 60.0f)

void GameObject::AttachObject(const GameObjectPtr &object) {
	Matrix4 objTransform = object->m_pNode->GetAbsTransform();
    Vector3 pos = objTransform * VectorZero;
    object->MoveObjectTo(pos);
    object->m_pBody->getWorldTransform().setFromOpenGLMatrix(objTransform.m);
    
    m_attachedObjects.insert(object);
}

void GameObject::MoveObjectBy(Vector3 relMove) {
    btTransform& trans = m_pBody->getWorldTransform();
    btVector3& vec = trans.getOrigin();
    vec.setX(vec.getX() + relMove.x);
    vec.setY(vec.getY() + relMove.y);
    vec.setZ(vec.getZ() + relMove.z);
    m_pNode->SetOrigin(vec);
    
    if (m_attachedObjects.size() > 0) {
        ObjectList::iterator it;
        for (it = m_attachedObjects.begin(); it != m_attachedObjects.end(); ++it) {
            (*it)->MoveBodyBy(relMove);
        }
    }
}

void GameObject::MoveBodyBy(Vector3 relMove) {
    btTransform& trans = m_pBody->getWorldTransform();
    btVector3& vec = trans.getOrigin();
	vec += relMove;
}

void GameObject::MoveObjectTo(Vector3 absMove) {
    btTransform& trans = m_pBody->getWorldTransform();
    btVector3& vec = trans.getOrigin();

    vec.setX(absMove.x);
    vec.setY(absMove.y);
    vec.setZ(absMove.z);

	ObjectList::iterator it;
	for (it = m_attachedObjects.begin(); it != m_attachedObjects.end(); ++it) {
		Vector3 relPos = (*it)->GetRelativePosition();
		(*it)->MoveObjectTo(relPos + vec);
	}

	if (!m_pNode->HasParent())
		m_pNode->SetOrigin(absMove);
}

void GameLogic::InitializeFromLevel(Level* pLevel) {
	m_pWorld = g_pPhysics->GenerateNewWorld();
	m_respawnPosition = Vector3(0.0f, 50.0f, 0.0f);
 
    ObjectIterator it = pLevel->ObjectsBegin();
    ObjectIterator end = pLevel->ObjectsEnd();
    
    while (it != end) {
        const ObjectDataPtr& data = (*it);
        AddObject(data.get());
        
        ++it;
    }

	while (!m_instanceNodeQueue.empty()) {
		InstancingNodePtr& node = m_instanceNodeQueue.front();
		node->InitData();
		m_instanceNodeQueue.pop();
	}
}

void GameLogic::SetDebugDrawer(PhysicsDebugDrawer *drawer) {
    //m_pWorld->setDebugDrawer(drawer);
}

void GameLogic::Update(float speed) {
    m_pWorld->stepSimulation(speed, 1, 1.0f / g_fMaxFPS);
    
    ControllerList::iterator it;
    for (it = m_controllers.begin(); it != m_controllers.end(); ++it) {
        (*it)->Update();
    }

	while (!m_removeQueue.empty()) {
		ControllerPtr& controller = m_removeQueue.front();
		controller->Release(this);
		controller->m_pGameLogic = nullptr;
		m_controllers.erase(controller);
		m_removeQueue.pop();
		OutputDebugStringW(L"Removed Controller\n");
	}
}

void GameLogic::Release() {
    ObjectMap::iterator it = m_objects.begin();
    ObjectMap::iterator end = m_objects.end();
    
    while (it != end) {
        GameObjectPtr object = it->second;
        RemoveObject((it++)->second);
		object = nullptr;
    }
    
	delete m_pWorld;
    m_pScene = nullptr;
}

GameObjectPtr GameLogic::AddObject(EDITOR_CONST LevelObjectData *data) {
#ifndef _WINDLL
	if (data->getFlag() == RESPAWN_FLAG) {
		m_respawnPosition = data->GetPosition();
		return nullptr;
	}
#endif

	GameObjectPtr object = GetObject(data->GetName());
	if (object != nullptr)
		return object;

    SceneNodePtr node = MakeNodePtr(data->MakeSceneNode());
	if (node) {
		node->SetOrigin(data->GetPosition());
		node->SetOrientation(data->GetRotation());
	}
    PhysicsShape* shape = data->MakeShape();
    PhysicsBody* body = g_pPhysics->MakeStaticBody(shape, data->GetPosition(), data->GetRotation());
    body->setRestitution(0.5f);
    body->setRollingFriction(1.0f);
    body->setFriction(1.0f);
    if (data->hasContactResponse())
        body->setCollisionFlags(body->getCollisionFlags() | PhysicsBody::CF_NO_CONTACT_RESPONSE);    

    if (data->m_parentName.compare("None") != 0) {
        object = AddToObject(GetObjectID(data->m_parentName), data->GetName(), body, node, data->MakeController());
		body->setUserPointer((void*)object.get());
#ifdef _WINDLL
		object->data = data;
#endif
    } else {
		IObjectController* controller = data->MakeController();
		object = AddObject(data->GetName(), body, node, controller);
#ifndef _WINDLL
		if (!object && FLAG_IS_PLATFORM(data->getFlag())) {
			PlatformNode* temp = (PlatformNode*)node.get();
			InstancingNodePtr inst = MakeInstancingNodePtr(DBG_NEW PlatformInstances(temp->mat));
			m_pScene->AddInstancingNode(inst);
			object = AddObject(data->GetName(), body, node, controller);
			m_instanceNodeQueue.push(inst);
		}
#endif
		body->setUserPointer((void*)object.get());

#ifdef _WINDLL
		object->data = data;
#endif
    }

	char output[128];
	sprintf_s(output, "Adding object: %s\n", data->m_name.c_str());

	OutputDebugStringA(output);

	return object;
}

void GameLogic::AddObject(const std::string &name, const GameObjectPtr& object) {
    ObjectID id = GetObjectID(name);
    
    m_pWorld->addRigidBody(object->m_pBody);
    m_pScene->AddNode(object->m_pNode);
    
    if (object->m_pController)
        AddController(object->m_pController);
}

GameObjectPtr GameLogic::AddObject(const std::string& name, PhysicsBody *body, const SceneNodePtr& node, IObjectController* controller) {

    
    ObjectID id = GetObjectID(name);
    ObjectMap::iterator it = m_objects.find(id);
    if (it != m_objects.end()) {
        printf("Object with name \"%s\" already exists\n", name.c_str());
        return NULL;
    }
    
    printf("Adding object with name \"%s\".\n", name.c_str());
    
	if (!m_pScene->AddNode(node))
		return nullptr;
	m_pWorld->addRigidBody(body);

    GameObjectPtr object = OBJECT_PTR(DBG_NEW GameObject);
    object->m_pBody = body;
    object->m_pNode = node;
    object->m_id = id;
#ifdef _WINDLL
	object->m_name = name;
#endif
    m_objects.insert(ObjectPair(id, object));
    
    if (controller) {
        ControllerPtr pController(controller);
        object->m_pController = pController;
        pController->m_pObject = object.get();
        AddController(pController);
    }
    
    return object;
}

void GameLogic::RemoveObject(ObjectID id) {
    ObjectMap::iterator it = m_objects.find(id);
    if (it != m_objects.end()) {
        GameObjectPtr& object = it->second;
        
        if (object->m_attachedObjects.size() > 0) {
            ObjectList::iterator it;
            for (it = object->m_attachedObjects.begin(); it != object->m_attachedObjects.end(); ++it) {
                RemoveObject((*it)->m_id);
            }
        }
        
        m_pWorld->removeRigidBody(object->m_pBody);
		//object->m_pBody->Release();
		//delete object->m_pBody;
        m_pScene->RemoveNode(object->m_pNode);
        RemoveController(object->m_pController);
        m_objects.erase(object->m_id);
    }
}

void GameLogic::RemoveObjectTemp(ObjectID id) {
	ObjectMap::iterator it = m_objects.find(id);
	if (it != m_objects.end()) {
		GameObjectPtr& object = it->second;

		if (object->m_attachedObjects.size() > 0) {
			ObjectList::iterator it;
			for (it = object->m_attachedObjects.begin(); it != object->m_attachedObjects.end(); ++it) {
				RemoveObject((*it)->m_id);
			}
		}

		m_pWorld->removeRigidBody(object->m_pBody);
		//object->m_pBody->Release();
		//delete object->m_pBody;
		m_pScene->RemoveNode(object->m_pNode);
		RemoveController(object->m_pController);
		m_objects.erase(object->m_id);
	}

}

GameObjectPtr GameLogic::AddToObject(ObjectID dest, const std::string &name, PhysicsBody *body, const SceneNodePtr &node, IObjectController *controller) {
    if (!body || !node)
        return NULL;
    
    ObjectID id = GetObjectID(name);
    ObjectMap::iterator it = m_objects.find(id);
    if (it != m_objects.end()) {
        printf("Object with name \"%s\" already exists\n", name.c_str());
        return NULL;
    }
    
    printf("Adding object with name \"%s\".\n", name.c_str());
    
    GameObjectPtr object = OBJECT_PTR(DBG_NEW GameObject);
    object->m_pBody = body;
    object->m_pNode = node;
    object->m_id = id;
#ifdef _WINDLL
	object->m_name = name;
#endif
    m_objects.insert(ObjectPair(id, object));
    
	m_pWorld->addRigidBody(body);
    
    if (controller) {
        ControllerPtr pController(controller);
        object->m_pController = pController;
        pController->m_pObject = object.get();
        AddController(pController);
    }
    
    if (object) {
        GameObjectPtr attachTo = GetObject(dest);
        attachTo->m_pNode->AddNode(node);
        attachTo->AttachObject(object);
    }
    
    return object;
}

GameObjectPtr GameLogic::MakeObject(PhysicsBody *body, const SceneNodePtr &node, IObjectController *controller) {
    if (!body || !node)
        return NULL;
    
    GameObjectPtr object(DBG_NEW GameObject);
    object->m_pBody = body;
    object->m_pNode = node;
    return object;
}

GameObject* GameLogic::ShootRay(const Ray& ray) {
	btCollisionWorld::ClosestRayResultCallback rayResult(ray.position, ray.direction);

	m_pWorld->rayTest(rayResult.m_rayFromWorld, rayResult.m_rayToWorld, rayResult);

	if (rayResult.hasHit()) {
		GameObject* object = (GameObject*)rayResult.m_collisionObject->getUserPointer();
		return object;
	}

	return nullptr;
}

Vector3 GameLogic::ShootRayPosition(const Ray& ray, GameObject** outObject) {
	btCollisionWorld::ClosestRayResultCallback rayResult(ray.position, ray.direction);

	m_pWorld->rayTest(rayResult.m_rayFromWorld, rayResult.m_rayToWorld, rayResult);

	if (rayResult.hasHit()) {
		if (outObject)
			*outObject = (GameObject*)rayResult.m_collisionObject->getUserPointer();
		return rayResult.m_hitPointWorld;
	}

	return VectorZero;
}


void GameLogic::SetGravity(const Vector3& grav) {
	m_pWorld->setGravity(grav);
}

void GameLogic::RemoveObjectWithAnim(ObjectID id, INodeAnimation* anim) {
	ObjectMap::iterator it = m_objects.find(id);
	if (it != m_objects.end()) {
		GameObjectPtr& object = it->second;

		if (object->m_attachedObjects.size() > 0) {
			ObjectList::iterator it;
			for (it = object->m_attachedObjects.begin(); it != object->m_attachedObjects.end(); ++it) {
				RemoveObject((*it)->m_id);
			}
		}

		m_pWorld->removeRigidBody(object->m_pBody);
		//object->m_pBody->Release();
		//delete object->m_pBody;
		anim->SetRemove(true);
		m_pScene->RunAnimation(NodeAnimationPtr(anim));
		RemoveController(object->m_pController);
		m_objects.erase(object->m_id);
	}
}