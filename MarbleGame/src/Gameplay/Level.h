//
//  Level.h
//  MarbleGame
//
//  Created by Tarik Karaca on 28.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__Level__
#define __MarbleGame__Level__

#include <vector>

#include "Maths/GeometryMath.h"
#include "Scene/SceneNode.h"
#include "Physics/Physics.h"
#include "CGraphics.h"

class PlatformNode;

class LevelObjectData;
class GameLogic;
class IObjectController;

#ifndef _WINDLL
#define GAME_ITER const_iterator
#else
#define GAME_ITER iterator
#endif

#ifndef _WINDLL
#define GAME_CONST const
#else
#define GAME_CONST
#endif

#define TILE_SCALING 1.0f

typedef std::shared_ptr<PlatformNode> PlatformNodePtr;

typedef std::shared_ptr<GAME_CONST LevelObjectData> ObjectDataPtr;

typedef std::vector<ObjectDataPtr>::GAME_ITER ObjectIterator;

class Level {
public:
    typedef std::vector<ObjectDataPtr> ObjectList;
    
    ObjectList m_objects;
	int m_starsCount;
public:
    void AddObject(const ObjectDataPtr&);
    void Release();
    
    void SaveLevel(const char* path);
    void LoadLevel(const char* path);
    
    ObjectList::GAME_ITER ObjectsBegin();
    ObjectList::GAME_ITER ObjectsEnd();
    int ObjectsCount();
    GAME_CONST ObjectDataPtr& ObjectByIndex(int);

	int StarsCount() const {
		return m_starsCount;
	}

	Level() {
		m_starsCount = 0;
	}
};

#define BUTTON_FLAG 'b'
#define TELEPORTER_FLAG 't'
#define TERRAIN_FLAG 'l'
#define WATER_FLAG 'w'
#define STAR_FLAG 's'
#define RESPAWN_FLAG 'z'
#define POWERUP_FLAG 'p'

class LevelObjectData {
    friend class Level;
    friend class GameLogic;
public:
    std::string m_name;
    std::string m_parentName;
public:
    virtual void Deserialize(FILE*) = 0;
    virtual void Serialize(FILE*) const = 0;
    
    virtual PhysicsShape* MakeShape() const = 0;
    virtual SceneNode* MakeSceneNode() const = 0;
    virtual IObjectController* MakeController() const { return nullptr; }
    
    virtual Vector3 GetPosition() const = 0;
    virtual Quaternion GetRotation() const = 0;

	virtual bool IsPlatform() const { return false; }

#ifdef _WINDLL
	virtual Vector3 GetPositionEditor() const {
		return GetPosition();
	}

	virtual void SetPosition(Vector3) = 0;
	virtual void SetRotation(Quaternion) = 0;
#endif
    
    const std::string& GetName() const {
        return m_name;
    }
    
    virtual char getFlag() const = 0;
    
    virtual bool hasContactResponse() const {
        return false;
    }
};

class ButtonData : public LevelObjectData {
public:
    Vector3 m_position;
    float m_rotation;
    
    
    void Deserialize(FILE*);
    void Serialize(FILE*) const;
    
    PhysicsShape* MakeShape() const;
    SceneNode* MakeSceneNode() const;
    IObjectController* MakeController() const;
    
    Vector3 GetPosition() const {
        return m_position;
    }
    
    Quaternion GetRotation() const {
        return Quaternion(VectorUp, m_rotation);
    }

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		m_position = position;
	}

	virtual void SetRotation(Quaternion q) {
		m_rotation = atan2f(2.0f * q.y * q.w - 2 * q.x * q.z, 1 - 2 * q.y * q.y - 2 * q.z * q.z) * 180.0f / 3.14159265f;
	}
#endif
    
    char getFlag() const {
        return BUTTON_FLAG;
    }
    
    bool hasContactResponse() const {
        return true;
    }
    
    ButtonData() { }
};

class TeleporterData : public LevelObjectData {
public:
	Vector3 m_position;
	std::string m_target;

	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;
	IObjectController* MakeController() const;

	Vector3 GetPosition() const {
		return m_position;
	}

	Quaternion GetRotation() const {
		return Quaternion();
	}

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		m_position = position;
	}

	virtual void SetRotation(Quaternion q) {

	}
#endif

	char getFlag() const {
		return TELEPORTER_FLAG;
	}

	bool hasContactResponse() const {
		return true;
	}

	TeleporterData() { }

};

class TerrainData : public LevelObjectData {
public:
	Vector3 m_position;

	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;
	IObjectController* MakeController() const { return nullptr; }

	Vector3 GetPosition() const { return VectorDown * 25.0f; /*return Vector3(1.0f, 1.0f, 0.0f);*/ }
	Quaternion GetRotation() const { return QUATERNION_IDENTITY; }

	char getFlag() const {
		return TERRAIN_FLAG;
	}

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		m_position = position;
	}

	virtual void SetRotation(Quaternion) { }
#endif

	bool hasContactResponse() const { return false; }

	TerrainData() { }
};

class WaterData : public LevelObjectData {
public:
	Vector3 m_position;

	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;
	IObjectController* MakeController() const;

	Vector3 GetPosition() const { return VectorUp * 2.0f; }
	Quaternion GetRotation() const { return QUATERNION_IDENTITY; }

	char getFlag() const {
		return WATER_FLAG;
	}

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		m_position = position;
	}

	virtual void SetRotation(Quaternion) { }
#endif

	bool hasContactResponse() const { return true; }

	WaterData() { }
};

class StarData : public LevelObjectData {
public:
	Vector3 position;

	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;
	IObjectController* MakeController() const;

	Vector3 GetPosition() const { return position; }
	Quaternion GetRotation() const {
		return QUATERNION_IDENTITY;
	}

	char getFlag() const {
		return STAR_FLAG;
	}

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		this->position = position;
	}

	virtual void SetRotation(Quaternion) { }
#endif

	bool hasContactResponse() const { return true; }

	StarData() { }
};

class RespawnData : public LevelObjectData {
public:
	Vector3 position;

	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;
	IObjectController* MakeController() const;

	Vector3 GetPosition() const { return position; }
	Quaternion GetRotation() const {
		return QUATERNION_IDENTITY;
	}

	char getFlag() const {
		return RESPAWN_FLAG;
	}

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		this->position = position;
	}

	virtual void SetRotation(Quaternion) { }
#endif

	bool hasContactResponse() const { return true; }

	RespawnData() { }
};

class PowerUpData : public LevelObjectData {
public:
	Vector3 position;

	void Deserialize(FILE*);
	void Serialize(FILE*) const;

	PhysicsShape* MakeShape() const;
	SceneNode* MakeSceneNode() const;
	IObjectController* MakeController() const;

	Vector3 GetPosition() const { return position; }
	Quaternion GetRotation() const {
		return QUATERNION_IDENTITY;
	}

	char getFlag() const {
		return POWERUP_FLAG;
	}

#ifdef _WINDLL
	virtual void SetPosition(Vector3 position) {
		this->position = position;
	}

	virtual void SetRotation(Quaternion) { }

#endif

	bool hasContactResponse() const { return true; }

	PowerUpData() { }
};




class ButtonNode : public ModelNode {
public:
    
    ButtonNode(Model* model, Material mat) : ModelNode(model, mat), y(0.0f) { }
    
    float y;
protected:
	void OnRender();
};

typedef std::shared_ptr<ButtonNode> ButtonNodePtr;

class GateNode : public SceneNode {
public:
	Model* gateModel;
	Texture2D* gateTexture;
	Vector2 texPos;

	GateNode();

protected:
	void OnAdded();
	void OnRender();
	void OnRelease();

	bool HasShadows() const { return false; }
	RenderPass GetRenderPass() const { return RenderPassPost; }

	CBaseEffect* m_pEffect;
};


class PowerUpNode : public SceneNode {
public:
	Model* model;
	Texture2D* texture;

	PowerUpNode() : SceneNode() { }
protected:
	void OnRender();
	void OnRelease();
	void OnAdded();

	bool HasShadows() const { return false; }
	RenderPass GetRenderPass() const { return RenderPassPost; }

	CBaseEffect* m_pEffect;
};



inline Level::ObjectList::GAME_ITER Level::ObjectsBegin() {
#ifndef _WINDLL
    return m_objects.cbegin();
#else
    return m_objects.begin();
#endif
}

inline Level::ObjectList::GAME_ITER Level::ObjectsEnd() {
#ifndef _WINDLL
    return m_objects.cend();
#else
    return m_objects.end();
#endif
}

inline int Level::ObjectsCount() {
    return m_objects.size();
}

inline GAME_CONST ObjectDataPtr& Level::ObjectByIndex(int index) {
    return m_objects.at(index);
}

#endif /* defined(__MarbleGame__Level__) */
