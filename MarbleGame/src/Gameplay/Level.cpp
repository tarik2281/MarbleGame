//
//  Level.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 28.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "Level.h"

#include "../Main.h"
#include "EventManager/Events.h"
#include "Physics/Physics.h"
#include "Resources/Model.h"
#include "Resources/MaterialList.h"
#include "Resources/ResourceManager.h"
#include "Scene/Terrain.h"
#include "Scene/InstancingNode.h"
#include "GameLogic.h"
#include "ButtonController.h"
#include "TeleporterController.h"
#include "Util.h"
#include "Scene/Scene.h"

#include "Platform.h"


#ifndef _WINDLL
#define Load QueueContent
#else
#define Load LoadContent
#endif



void Level::AddObject(const ObjectDataPtr &data) {
    m_objects.push_back(data);

	if (data->getFlag() == STAR_FLAG) {
		m_starsCount++;
	}
}

void Level::Release() {
    
}



void Level::SaveLevel(const char *path) {
    FILE* fp = fopen(path, "w");
    
    ObjectList::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); ++it) {
        fprintf(fp, "%c ", (*it)->getFlag());
        (*it)->Serialize(fp);
        fprintf(fp, "\n");
    }
    
    fclose(fp);
}

void Level::LoadLevel(const char *path) {
    Release();
    FILE* fp = fopen(path, "r");
    while (1) {
        char lineHeader[128];
        int res = fscanf(fp, "%s", lineHeader);
        if (res == EOF) break;
        
        char objectFlag = lineHeader[0];
        switch (objectFlag) {
            case PLATFORM_FLAG: {
                PlatformData* data = DBG_NEW PlatformData;
                data->Deserialize(fp);
                AddObject(ObjectDataPtr(data));
                break;
            }
            case RAMP_FLAG: {
                RampData* data = DBG_NEW RampData;
                data->Deserialize(fp);
                AddObject(ObjectDataPtr(data));
                break;
            }
            case BUTTON_FLAG: {
                ButtonData* data = DBG_NEW ButtonData;
                data->Deserialize(fp);
                AddObject(ObjectDataPtr(data));
                break;
            }
            case TELEPORTER_FLAG: {
                TeleporterData* data = DBG_NEW TeleporterData;
                data->Deserialize(fp);
                AddObject(ObjectDataPtr(data));
                break;
            }
			case TERRAIN_FLAG: {
				TerrainData* data = DBG_NEW TerrainData;
				data->Deserialize(fp);
				AddObject(ObjectDataPtr(data));
				break;
			}
			case WATER_FLAG: {
				WaterData* data = DBG_NEW WaterData;
				data->Deserialize(fp);
				AddObject(ObjectDataPtr(data));
				break;
			}
			case STAR_FLAG: {
				StarData* data = DBG_NEW StarData;
				data->Deserialize(fp);
				AddObject(ObjectDataPtr(data));
				break;
			}
			case RESPAWN_FLAG: {
				RespawnData* data = DBG_NEW RespawnData;
				data->Deserialize(fp);
				AddObject(ObjectDataPtr(data));
				break;
			}
			case POWERUP_FLAG: {
				PowerUpData* data = DBG_NEW PowerUpData;
				data->Deserialize(fp);
				AddObject(ObjectDataPtr(data));
				break;
			}
			case CORNER_FLAG: {
								  CornerData* data = DBG_NEW CornerData;
								  data->Deserialize(fp);
								  AddObject(ObjectDataPtr(data));
								  break;
			}
            default:
                break;
        }
    }
    fclose(fp);
}

void ButtonData::Deserialize(FILE *file) {
    char name[100];
    char parentName[100];
    
    fscanf(file, "%s %s %f %f %f", name, parentName, &m_position.x, &m_position.y, &m_position.z);
    
    m_name = name;
    m_parentName = parentName;
	m_rotation = 0.0f;
}

void ButtonData::Serialize(FILE *file) const {
    fprintf(file, "%s %s %f %f %f", m_name.c_str(), m_parentName.c_str(), m_position.x, m_position.y, m_position.z);
}

PhysicsShape* ButtonData::MakeShape() const {
    PhysicsShape* shape = g_pPhysics->MakeCylinderShape(0.30704f / 1.4f, 0.1903f / 2.0f);
    return shape;
}

SceneNode* ButtonData::MakeSceneNode() const {
    Material pMat = g_pMaterials->GetVisualMaterial("Button");
    Model* model = g_pResourceManager->Load<Model>("Button.obj");
    ButtonNode* node = DBG_NEW ButtonNode(model, pMat);
	node->SetOrigin(m_position);
	node->SetOrientation(GetRotation());
    return node;
}

IObjectController* ButtonData::MakeController() const {
#ifndef EDITOR
    ButtonController* controller = DBG_NEW ButtonController;
    controller->SetMoveUp(m_name == "Button1");
    controller->m_remove = m_name == "Button3";
    return controller;
#else 
    return nullptr;
#endif
}


void TeleporterData::Deserialize(FILE *file) {
    char name[100];
    char parentName[100];
    char targetName[100];
    
    fscanf(file, "%s %s %s %f %f %f", name, parentName, targetName, &m_position.x, &m_position.y, &m_position.z);
    
    m_name = name;
    m_parentName = parentName;
    m_target = targetName;
}

void TeleporterData::Serialize(FILE *file) const {
    fprintf(file, "%s %s %s %f %f %f", m_name.c_str(), m_parentName.c_str(), m_target.c_str(), m_position.x, m_position.y, m_position.z);
}

PhysicsShape* TeleporterData::MakeShape() const {
    PhysicsShape* shape = g_pPhysics->MakeCylinderShape(0.75f / 1.4f, 0.62f);
    return shape;
}

SceneNode* TeleporterData::MakeSceneNode() const {
	GateNode* node = DBG_NEW GateNode();
	node->gateModel = g_pResourceManager->Load<Model>("Gate.obj");
	node->gateTexture = g_pResourceManager->Load<Texture2D>("GateTexture.png");
	return node;
}

IObjectController* TeleporterData::MakeController() const {
#ifndef EDITOR
    TeleporterController* controller = DBG_NEW TeleporterController;
    controller->m_targetName = m_target;
    return controller;
#else
    return nullptr;
#endif
}


struct TerrainVertex {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
};

void TerrainData::Deserialize(FILE* file) {
	char name[100];
	char parentName[100];

	fscanf(file, "%s %s", name, parentName);

	m_name = name;
	m_parentName = parentName;
}

void TerrainData::Serialize(FILE* file) const {
	fprintf(file, "%s %s", m_name.c_str(), m_parentName.c_str());
}

SceneNode* TerrainData::MakeSceneNode() const {
	TerrainNode* node = DBG_NEW TerrainNode;
	node->init();
	return node;

	/*Model* model = g_pResourceManager->QueueContent<Model>("CurvePlatform.obj");
	Material mat = g_pMaterials->GetVisualMaterial("Wood");
	ModelNode* node = DBG_NEW ModelNode(model, mat);
	return node;*/
}

#include <stdint.h>

PhysicsShape* TerrainData::MakeShape() const {
	btTriangleMesh* mesh = new btTriangleMesh;

	std::string terrainPath = ResourceManager::GetResourcesPath() + "Terrain.dat";
	FILE* stream = fopen(terrainPath.c_str(), "rb");

	int blockSize;
	fread(&blockSize, sizeof(int), 1, stream);
	TerrainVertex* vertexArray = new TerrainVertex[blockSize];
	fread(vertexArray, sizeof(TerrainVertex), blockSize, stream);

	for (int i = 0; i < blockSize; i++) {
		vertexArray[i].position *= 0.5f;
	}

	unsigned int indexBufferSize;
	fread(&indexBufferSize, sizeof(unsigned int), 1, stream);
	uint16_t* indices = new uint16_t[indexBufferSize];
	fread(indices, sizeof(uint16_t), indexBufferSize, stream);

	fclose(stream);

	for (unsigned int i = 0; i < indexBufferSize; i += 3) {
		mesh->addTriangle(vertexArray[indices[i]].position, vertexArray[indices[i + 1]].position, vertexArray[indices[i + 2]].position);
	}

	delete[] indices;
	delete[] vertexArray;

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);
	shape->setUserPointer(mesh);

	return shape;

	/*btTriangleMesh* mesh = new btTriangleMesh;

	FILE* file = fopen((ResourceManager::GetResourcesPath() + "CurvePlatform.obj").c_str(), "r");

	std::vector<Vector3> vertices;

	while (true) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) break;

		if (strcmp(lineHeader, "v") == 0) {
			Vector3 v;
			fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
			vertices.push_back(v);
		}

		if (strcmp(lineHeader, "f") == 0) {
			unsigned short vertexIndex[3], normalIndex[3], texelIndex[3];
			fscanf(file, "%hd/%hd/%hd %hd/%hd/%hd %hd/%hd/%hd\n",
				&vertexIndex[0], &texelIndex[0], &normalIndex[0],
				&vertexIndex[1], &texelIndex[1], &normalIndex[1],
				&vertexIndex[2], &texelIndex[2], &normalIndex[2]);

			mesh->addTriangle(vertices[vertexIndex[0] - 1], vertices[vertexIndex[1] - 1], vertices[vertexIndex[2] - 1]);
		}
	}

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);
	shape->setUserPointer(mesh);
	return shape;*/

	/*btTriangleMesh* mesh = new btTriangleMesh;

	float m_radius = 25.0f;

	float inRadius = m_radius - 2.0f;
	float outRadius = m_radius + 2.0f;

	float halfHeight = 5.0f;

	{
		Vector2 v1(cosf(0) * inRadius, sinf(0) * inRadius);
		Vector2 v2(cosf(0) * outRadius, sinf(0) * outRadius);
		mesh->addTriangle(btVector3(v2.x, -halfHeight, v2.y), btVector3(v1.x, -halfHeight, v1.y), btVector3(v2.x, halfHeight, v2.y));
		mesh->addTriangle(btVector3(v2.x, halfHeight, v2.y), btVector3(v1.x, -halfHeight, v1.y), btVector3(v1.x, halfHeight, v1.y));
	}

	float step = 90.0f / 6.0f;
	for (float f = 0.0f; f < 89.0f; f += step) {
		Vector2 v1(cosf(f * M_PI / 180.0f) * inRadius, sinf(f * M_PI / 180.0f) * inRadius);
		Vector2 v2(cosf(f * M_PI / 180.0f + step * M_PI / 180.0f) * inRadius, sinf(f * M_PI / 180.0f + step * M_PI / 180.0f) * inRadius);
		mesh->addTriangle(btVector3(v1.x, -halfHeight, v1.y), btVector3(v2.x, -halfHeight, v2.y), btVector3(v1.x, halfHeight, v1.y));
		mesh->addTriangle(btVector3(v1.x, halfHeight, v1.y), btVector3(v2.x, -halfHeight, v2.y), btVector3(v2.x, halfHeight, v2.y));
	}

	{
		Vector2 v1(cosf(90 * M_PI / 180.0f) * inRadius, sinf(90 * M_PI / 180.0f) * inRadius);
		Vector2 v2(cosf(90 * M_PI / 180.0f) * outRadius, sinf(90 * M_PI / 180.0f) * outRadius);
		mesh->addTriangle(btVector3(v1.x, -halfHeight, v1.y), btVector3(v2.x, -halfHeight, v2.y), btVector3(v1.x, halfHeight, v1.y));
		mesh->addTriangle(btVector3(v1.x, halfHeight, v1.y), btVector3(v2.x, halfHeight, v2.y), btVector3(v1.x, halfHeight, v1.y));
	}

	for (float f = 90.0f; f > 0.0f; f -= step) {
		Vector2 v1(cosf(f * M_PI / 180.0f) * outRadius, sinf(f * M_PI / 180.0f) * outRadius);
		Vector2 v2(cosf(f * M_PI / 180.0f + step * M_PI / 180.0f) * outRadius, sinf(f * M_PI / 180.0f + step * M_PI / 180.0f) * outRadius);
		mesh->addTriangle(btVector3(v1.x, -halfHeight, v1.y), btVector3(v2.x, -halfHeight, v2.y), btVector3(v1.x, halfHeight, v1.y));
		mesh->addTriangle(btVector3(v1.x, halfHeight, v1.y), btVector3(v2.x, -halfHeight, v2.y), btVector3(v2.x, halfHeight, v2.y));
	}

	for (float f = 0.0f; f < 89.0f; f += step) {
		Vector2 v1(cosf(f * M_PI / 180.0f) * inRadius, sinf(f * M_PI / 180.0f) * inRadius);
		Vector2 v2(cosf(f * M_PI / 180.0f) * outRadius, sinf(f * M_PI / 180.0f) * outRadius);
		Vector2 v3(cosf(f * M_PI / 180.0f + step * M_PI / 180.0f) * inRadius, sinf(f * M_PI / 180.0f + step * M_PI / 180.0f) * inRadius);
		Vector2 v4(cosf(f * M_PI / 180.0f + step * M_PI / 180.0f) * outRadius, sinf(f * M_PI / 180.0f + step * M_PI / 180.0f) * outRadius);
		mesh->addTriangle(btVector3(v1.x, halfHeight, v1.y), btVector3(v2.x, halfHeight, v2.y), btVector3(v3.x, halfHeight, v3.y));
		mesh->addTriangle(btVector3(v3.x, halfHeight, v3.y), btVector3(v4.x, halfHeight, v4.y), btVector3(v2.x, halfHeight, v2.y));
	}

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);
	shape->setUserPointer(mesh);

	return shape;*/
}


void WaterData::Deserialize(FILE* file) {
	char name[100];
	char parentName[100];

	fscanf(file, "%s %s", name, parentName);

	m_name = name;
	m_parentName = parentName;
}

void WaterData::Serialize(FILE * file) const {
	fprintf(file, "%s %s", m_name.c_str(), m_parentName.c_str());
}

SceneNode* WaterData::MakeSceneNode() const {
	WaterNode* node = DBG_NEW WaterNode;
	node->init();
	return node;
}

PhysicsShape* WaterData::MakeShape() const {
	PhysicsShape* shape = g_pPhysics->MakeBoxShape(Vector3(62.5f, 2.0f, 62.5f));
	return shape;
}


#include <math.h>

class WaterController : public IObjectController, public IEventListener {
	bool m_isInWater;
public:
	void Initialize(GameLogic*) {
		g_pEventManager->AddEventListener(this, CollisionEvent::sk_eventType);
		WaterNode* node = static_cast<WaterNode*>(m_pObject->GetSceneNode().get());
		node->wavesPosition = Vector2(0.0f, 0.0f);
		m_isInWater = false;
	}
	void Update() {
		WaterNode* node = static_cast<WaterNode*>(m_pObject->GetSceneNode().get());

		Vector2& wavesPosition = node->wavesPosition;

		node->wavesPosition.x += 0.001f * 2.0f; // 0.002 / frame * 60 = 0.12
		node->wavesPosition.y += 0.0005f * 2.0f; // 0.001 / frame * 60 = 0.06

		if (node->wavesPosition.x > 1.0f)
			node->wavesPosition.x = 0.0f;
		if (node->wavesPosition.y > 1.0f)
			node->wavesPosition.y = 0.0f;

		if (m_isInWater) {
			GameObjectPtr player = m_pGameLogic->GetObject(m_pGameLogic->GetObjectID("Player"));
			Vector3 playerPos = player->GetAbsolutePosition();
			Vector3 objectPos = m_pObject->GetAbsolutePosition();
			float boundingVolume = (m_pGameLogic->m_isSmall ? 1 : 2 * 2 * 2);
			if (m_pGameLogic->m_isSmall) {
				boundingVolume -= max(((playerPos.y + 0.5f) - (objectPos.y + 2.0f)), 0.0f);
			}
			else {
				boundingVolume -= max(((playerPos.y + 1.0f) - (objectPos.y + 2.0f)), 0.0f) * 2 * 2;
			}
			printf("diff = %f\n", ((playerPos.y + 1.0f) - (objectPos.y + 2.0f)));
			float volInCM3 = boundingVolume * 1000;
			float force = volInCM3 * 0.001f * (-(DEFAULT_GRAVITY));
			Vector3 sumForce = VectorZero;
			sumForce.y = force;
			player->GetPhysicsBody()->applyCentralForce(sumForce);
		}

	}
	void Release(GameLogic*) { g_pEventManager->RemoveEventListener(this, CollisionEvent::sk_eventType); }

	void HandleEvent(const EventDataPtr& event) {
		if (event->getEventType() == CollisionEvent::sk_eventType) {
			CollisionEvent* data = static_cast<CollisionEvent*>(event.get());
			if (IS_COLL_BODY(data, m_pObject->GetPhysicsBody())) {
				GameObjectPtr player = m_pGameLogic->GetObject(m_pGameLogic->GetObjectID("Player"));
				if (IS_COLL_BODY(data, player->GetPhysicsBody())) {
					player->GetPhysicsBody()->setDamping((data->m_added ? 0.5f : 0.0f), (data->m_added ? 0.5f : 0.0f));
					m_isInWater = data->m_added;
				}
			}
		}
	}
};

IObjectController* WaterData::MakeController() const {
#ifndef EDITOR
	return DBG_NEW WaterController;
#else
	return nullptr;
#endif
}


void StarData::Deserialize(FILE* stream) {
	char name[100];
	char parentName[100];

	fscanf(stream, "%s %s %f %f %f", name, parentName, &position.x, &position.y, &position.z);

	m_name = name;
	m_parentName = parentName;
}

void StarData::Serialize(FILE* stream) const {
	fprintf(stream, "%s %s %f %f %f", m_name.c_str(), m_parentName.c_str(), position.x, position.y, position.z);
}

PhysicsShape* StarData::MakeShape() const {
	return g_pPhysics->MakeSphereShape(0.52f);
}

SceneNode* StarData::MakeSceneNode() const {
	StarNode* node = DBG_NEW StarNode();
	return node;
}

class StarController : public IObjectController, public IEventListener {
	bool m_collected;
	static const Quaternion sk_step;
public:
	void Initialize(GameLogic*) {
		g_pEventManager->AddEventListener(this, CollisionEvent::sk_eventType);
		m_collected = false;
	}

	void Update() {
		if (m_collected) {
			ScaleNodeAnimation* anim = DBG_NEW ScaleNodeAnimation(m_pObject->GetSceneNode(), 10.0f / 60.0f, 1.0f, 0.0f);
			m_pGameLogic->RemoveObjectWithAnim(m_pObject->GetIdentifier(), anim);
		}
	}

	void Release(GameLogic*) {
		g_pEventManager->RemoveEventListener(this, CollisionEvent::sk_eventType);
	}

	void HandleEvent(const EventDataPtr& data) {
		CollisionEvent* event = static_cast<CollisionEvent*>(data.get());
		if (IS_COLL_BODY(event, m_pObject->GetPhysicsBody())) {
			if (!m_collected) {
				StarCollectEvent* event = DBG_NEW StarCollectEvent();
				g_pEventManager->QueueEvent(EventDataPtr(event));
			}
			//m_pGameLogic->RemoveObject(m_pObject->GetIdentifier());
			m_collected = true;
		}
	}
};

const Quaternion StarController::sk_step = Quaternion(VectorUp, 2.0f);

IObjectController* StarData::MakeController() const {
#ifndef EDITOR
	return DBG_NEW StarController;
#else
	return nullptr;
#endif
}


void RespawnData::Deserialize(FILE* stream) {
	m_name = "Respawn";
	m_parentName = "None";
	fscanf(stream, "%f %f %f", &position.x, &position.y, &position.z);
}

void RespawnData::Serialize(FILE* stream) const {
	fprintf(stream, "%f %f %f", position.x, position.y, position.z);
}

PhysicsShape* RespawnData::MakeShape() const {
#ifndef _WINDLL
	return nullptr;
#else
	return g_pPhysics->MakeSphereShape(1.0f);
#endif
}

SceneNode* RespawnData::MakeSceneNode() const {
#ifndef _WINDLL
	return nullptr;
#else
	Material mat = g_pMaterials->GetVisualMaterial("Soccer");
	Model* model = g_pResourceManager->LoadContent<Model>("Marble4.obj");
	return new ModelNode(model, mat);
#endif
}

IObjectController* RespawnData::MakeController() const {
	return nullptr;
}

void ButtonNode::OnRender() {
	TEMP_CONTEXT(pContext);

	if (m_mat.diffuseMap && m_mat.normalMap) {
		ID3D11ShaderResourceView* resources[2] = {
			m_mat.diffuseMap->m_pView, m_mat.normalMap->m_pView
		};
		pContext->PSSetShaderResources(0, 2, resources);
	}

    Mesh* mesh = m_pModel->Begin();
    
    do {
        if (strcmp(mesh->name.c_str(), "Plane") != 0) {
			m_pScene->GetObjectData()->mModel = (GetAbsTransform() * MatrixTranslate(VectorUp * -y)).Transpose();
			m_pScene->UpdateObjectData();
        }
		else {
			m_pScene->GetObjectData()->mModel = GetAbsTransform().Transpose();
			m_pScene->UpdateObjectData();
		}

		EffectManager->ApplyEffect(m_pEffect);
        mesh->Render();
    } while ((mesh = m_pModel->Next()));
}


GateNode::GateNode() : SceneNode() {
	
}

void GateNode::OnAdded() {
	m_pEffect = EffectManager->LoadEffect("GateShader");
}

void GateNode::OnRender() {
	TEMP_CONTEXT(pContext);

	pContext->PSSetShaderResources(0, 1, &gateTexture->m_pView);
	EffectManager->ApplyEffect(m_pEffect);
	g_pGraphics->EnableBlending();
	gateModel->Render();
	g_pGraphics->DisableBlending();
}

void GateNode::OnRelease() {
	OutputDebugStringA("Release Gate Node\n");
}


void PowerUpNode::OnRender() {
	TEMP_CONTEXT(pContext);

	pContext->PSSetShaderResources(0, 1, &texture->m_pView);
	EffectManager->ApplyEffect(m_pEffect);
	g_pGraphics->EnableBlending();
	model->Render();
	g_pGraphics->DisableBlending();
}

void PowerUpNode::OnRelease() {
	
}

void PowerUpNode::OnAdded() {
	m_pEffect = EffectManager->LoadEffect("ShaderSimple");
}


void PowerUpData::Deserialize(FILE* stream) {
	char name[100];
	char parentName[100];

	fscanf(stream, "%s %s %f %f %f", name, parentName, &position.x, &position.y, &position.z);

	m_name = name;
	m_parentName = parentName;
}

void PowerUpData::Serialize(FILE* stream) const {
	fprintf(stream, "%s %s %f %f %f", m_name.c_str(), m_parentName.c_str(), position.x, position.y, position.z);
}

PhysicsShape* PowerUpData::MakeShape() const {
	return g_pPhysics->MakeSphereShape(0.52f);
}

SceneNode* PowerUpData::MakeSceneNode() const {
	PowerUpNode* node = DBG_NEW PowerUpNode();
	node->model = g_pResourceManager->Load<Model>("PowerUp.obj");
	node->texture = g_pResourceManager->Load<Texture2D>("PowerUp.png");
	return node;
}

class PowerUpController : public IObjectController, public IEventListener {
	bool m_collected;
public:
	void Initialize(GameLogic* logic) {
		g_pEventManager->AddEventListener(this, CollisionEvent::sk_eventType);
		m_collected = false;
	}

	void Update() {
		m_pObject->GetSceneNode()->SetOrientation(m_pObject->GetSceneNode()->GetOrientation() * Quaternion(Vector3(0.0f, 1.0f, 0.0f), 2.0f));

		if (m_collected) {
			ScaleNodeAnimation* anim = new ScaleNodeAnimation(m_pObject->GetSceneNode(), 20.0f / 60.0f, 1.0f, 10.0f);
			m_pGameLogic->RemoveObjectWithAnim(m_pObject->GetIdentifier(), anim);
			g_pEventManager->QueueEvent(EventDataPtr(DBG_NEW PowerUpCollectEvent()));
		}
	}

	void Release(GameLogic* logic) {
		g_pEventManager->RemoveEventListener(this, CollisionEvent::sk_eventType);
	}

	void HandleEvent(const EventDataPtr& data) {
		CollisionEvent* event = static_cast<CollisionEvent*>(data.get());
		if (IS_COLL_BODY(event, m_pObject->GetPhysicsBody())) {
			m_collected = true;
		}
	}
};

IObjectController* PowerUpData::MakeController() const {
	return DBG_NEW PowerUpController;
}