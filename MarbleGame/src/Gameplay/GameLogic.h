//
//  GameLogic.h
//  MarbleGame
//
//  Created by Tarik Karaca on 06.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__GameLogic__
#define __MarbleGame__GameLogic__

class PhysicsBody;
class Scene;
class SceneNode;

#include <set>
#include <queue>

#include "Physics/Physics.h"
#include "Maths/VectorMath.h"
#include "Maths/GeometryMath.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "StringHash.h"

class GameLogic;
class IObjectController;
struct GameObject;
class Level;
class LevelObjectData;

typedef std::shared_ptr<IObjectController> ControllerPtr;
typedef std::shared_ptr<GameObject> GameObjectPtr;

#define OBJECT_PTR(object) GameObjectPtr(object, GameObject::ObjectDeleter())

typedef std::set<GameObjectPtr> ObjectList;

#ifdef _WINDLL
#define EDITOR_CONST
#else
#define EDITOR_CONST const
#endif

struct PhysicsMaterial {
    
};

typedef Hash ObjectID;
#define NO_ID 0

class GameObject {
public:
	struct ObjectSorter {
		bool operator()(const GameObjectPtr& first, const GameObjectPtr& second) {
			return (first->GetIdentifier() < second->GetIdentifier());
		}
	};

	struct ObjectDeleter {
		void operator()(GameObject* object) {
			SAFE_RELEASE(object->m_pBody);
			object->m_pNode = nullptr;
			object->m_pController = nullptr;
			delete object;
		}
	};

	ObjectID GetIdentifier() const { return m_id; }
	PhysicsBody* GetPhysicsBody() const { return m_pBody; }
	SceneNodePtr GetSceneNode() const { return m_pNode; }
	ControllerPtr GetController() const { return m_pController; }

#ifdef _WINDLL
	LevelObjectData* data;
#endif
    
    void AttachObject(const GameObjectPtr&);
    void DetachObject(const GameObjectPtr&);
    
    void MoveObjectBy(Vector3);
    void MoveObjectTo(Vector3);
    
    Vector3 GetRelativePosition() const {
        return m_pNode->GetOrigin();
    }
    
    Vector3 GetAbsolutePosition() const {
        return m_pBody->getWorldTransform().getOrigin();
    }

	GameObject() {
		m_pController = nullptr;
	}

	~GameObject() {
		//OutputDebugStringW(L"Game Object deallocated.\n");
	}
private:
	friend class GameLogic;

    void MoveBodyBy(Vector3);

	ObjectID m_id;
	PhysicsBody* m_pBody;
	SceneNodePtr m_pNode;
	ControllerPtr m_pController;
	ObjectList m_attachedObjects;
};

class IObjectController {
    friend class GameLogic;
protected:
    GameLogic* m_pGameLogic;
public:
    GameObject* m_pObject; // controlled object by this
    
    virtual void Initialize(GameLogic*) = 0;
    virtual void Update() = 0;
    virtual void Release(GameLogic*) = 0;
};


class GameLogic {
public:
    typedef std::map<ObjectID, GameObjectPtr> ObjectMap;
    typedef std::pair<ObjectID, GameObjectPtr> ObjectPair;
    typedef std::set<ControllerPtr> ControllerList;
	typedef std::queue<ControllerPtr> ControllerQueue;

	typedef std::queue<InstancingNodePtr> LoadInstsQueue;
    
    ObjectMap m_objects;
    ControllerList m_controllers;
	ControllerQueue m_removeQueue;
	LoadInstsQueue m_instanceNodeQueue;
    Scene* m_pScene;
    PhysicsWorld* m_pWorld;
public:
	Vector3 m_respawnPosition;
	bool m_isSmall;
    
    void InitializeFromLevel(Level*);
    void SetDebugDrawer(PhysicsDebugDrawer*);
    void DebugDrawWorld();
    
    void Update(float speed = 1.0f / 60.0f);
    
    void Release();
    
    void RemoveObject(GameObjectPtr);
    void RemoveObject(ObjectID);
	void RemoveObjectTemp(ObjectID);
	void RemoveObjectWithAnim(ObjectID, INodeAnimation*);
    void RemoveObject(const std::string& name) {
        ObjectID id = GetObjectID(name);
        RemoveObject(id);
    }
    
    GameObjectPtr GetObject(ObjectID id) {
        ObjectMap::iterator it = m_objects.find(id);
        return (it != m_objects.end()) ? it->second : nullptr;
    }

	GameObjectPtr GetObject(const std::string& name) {
		ObjectID id = GetObjectID(name);
		return GetObject(id);
	}
    
    ObjectID GetObjectID(const std::string& name) {
        return StringHash(name.c_str());
    }
    
    bool ObjectExists(const std::string& name) {
        ObjectMap::iterator it = m_objects.find(GetObjectID(name));
        return it != m_objects.end();
    }
    
    GameObjectPtr MakeObject(PhysicsBody*, const SceneNodePtr&, IObjectController* = 0);
    
    void AddObject(const std::string&, const GameObjectPtr&);
    GameObjectPtr AddObject(const std::string&, PhysicsBody*, const SceneNodePtr&, IObjectController* = 0);
    void AddToObject();
    GameObjectPtr AddToObject(ObjectID, const std::string&, PhysicsBody*, const SceneNodePtr&, IObjectController* = 0);
    
    GameObjectPtr AddObject(EDITOR_CONST LevelObjectData*);
    
    void AddController(const ControllerPtr&);
    void RemoveController(const ControllerPtr&);
    
    void DoCommand();

	GameObject* ShootRay(const Ray&);
	Vector3 ShootRayPosition(const Ray&, GameObject** = 0);
    
	GameLogic(Scene* pScene) : m_pScene(pScene) { m_isSmall = false; }
    ~GameLogic() { printf("Game logic deallocation.\n"); }

	void SetGravity(const Vector3&);
};


inline void GameLogic::DebugDrawWorld() {
    m_pWorld->debugDrawWorld();
}

inline void GameLogic::AddController(const ControllerPtr& controller) {
    if (!controller)
        return;
    
    controller->m_pGameLogic = this;
    controller->Initialize(this);
    m_controllers.insert(controller);
}

inline void GameLogic::RemoveController(const ControllerPtr &controller) {
    if (!controller)
        return;

	m_removeQueue.push(controller);
}

inline void GameLogic::RemoveObject(GameObjectPtr object) {
    if (!object)
        return;  
    
	m_pWorld->removeRigidBody(object->GetPhysicsBody());
	//delete object->m_pBody;
	m_pScene->RemoveNode(object->GetSceneNode());
	
	if (object->GetController()) {
		RemoveController(object->GetController());
		//object->m_pController = nullptr;
	}

    m_objects.erase(object->GetIdentifier());
}

#endif /* defined(__MarbleGame__GameLogic__) */
