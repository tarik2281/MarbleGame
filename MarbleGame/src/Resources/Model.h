//
//  Model.h
//  MarbleGame
//
//  Created by Tarik Karaca on 04.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__Model__
#define __MarbleGame__Model__

#include <string>
#include <vector>

#include "Resource.h"
#include "Maths/VectorMath.h"
#include "CGraphics.h"

class Model;

struct Mesh {
    std::string name;

    void Render();
public:
    int indexBufferOffset;
    int indicesSize;
    Model* model;
    
    friend class Model;
};

struct Vertex {
	Vector3 position;
	Vector2 texCoord;
	Vector3 normal;
	Vector4 tangent;
};

class Model : public IResource {
    typedef std::vector<Mesh> MeshList;

    int m_index;
	Vector3 m_vMin;
	Vector3 m_vMax;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	MeshList m_meshes;
public:
	const Vector3& GetMin() const { return m_vMin; }
	const Vector3& GetMax() const { return m_vMax; }

    Mesh* Begin();
    Mesh* Next();
    void Render();
    
    void LoadResource(const char* path);
    void Release();
    void Copy(IResource* destination);
};

#endif /* defined(__MarbleGame__Model__) */
