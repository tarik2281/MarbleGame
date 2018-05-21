//
//  Model.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 04.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "Model.h"
#include "Util.h"

void Mesh::Render() {
	g_pGraphics->GetContext()->DrawIndexed(indicesSize, indexBufferOffset, 0);
}

void Model::LoadResource(const char* path) {
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;
	std::vector<Vertex> outVertices;
	std::vector<unsigned short> indices;

	Mesh* currentMesh = 0;

	FILE* file = fopen(path, "r");
	
	m_vMin = Vector3(FLT_MAX);
	m_vMax = Vector3(-FLT_MAX);

	while (true) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) break;

		if (strcmp(lineHeader, "#") == 0) continue;
		else if (strcmp(lineHeader, "v") == 0) {
			Vector3 v;
			fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
			vertices.push_back(v);
			m_vMin = m_vMin.Min(v);
			m_vMax = m_vMax.Max(v);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			Vector3 v;
			fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
			normals.push_back(v);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			Vector2 v;
			fscanf(file, "%f %f\n", &v.x, &v.y);
			texCoords.push_back(v);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned short vertexIndex[3], normalIndex[3], texelIndex[3];
			fscanf(file, "%hd/%hd/%hd %hd/%hd/%hd %hd/%hd/%hd\n",
				&vertexIndex[0], &texelIndex[0], &normalIndex[0],
				&vertexIndex[1], &texelIndex[1], &normalIndex[1],
				&vertexIndex[2], &texelIndex[2], &normalIndex[2]);

			for (int i = 0; i < 3; i++) {
				Vertex v;
				v.position = vertices[vertexIndex[i] - 1];
				v.normal = normals[normalIndex[i] - 1];
				v.texCoord = texCoords[texelIndex[i] - 1];

				unsigned short index = 0;

				for (index = 0; index < outVertices.size(); index++) {
					if (v.position == outVertices[index].position &&
						v.normal == outVertices[index].normal &&
						v.texCoord == outVertices[index].texCoord) {
						indices.push_back(index);

						if (currentMesh)
							currentMesh->indicesSize++;
						break;
					}
				}
				if (index >= outVertices.size()) {
					outVertices.push_back(v);
					indices.push_back(index);

					if (currentMesh)
						currentMesh->indicesSize++;
				}
			}
		}
		else if (strcmp(lineHeader, "o") == 0) {
			char name[128];
			fscanf(file, "%s\n", name);
			Mesh mesh;
			mesh.name = name;
			mesh.model = this;
			mesh.indexBufferOffset = indices.size();
			mesh.indicesSize = 0;
			m_meshes.push_back(mesh);
			currentMesh = &m_meshes.back();
		}
	}

	if (!currentMesh) {
		Mesh mesh;
		mesh.name = "No Name";
		mesh.model = this;
		mesh.indexBufferOffset = 0;
		mesh.indicesSize = indices.size();
		m_meshes.push_back(mesh);
	}


	// Tangent space calculation
	int triangleCount = indices.size() / 3;
	Vector3* tan1 = DBG_NEW Vector3[outVertices.size() * 2];
	Vector3* tan2 = tan1 + outVertices.size();
	memset(tan1, 0, sizeof(Vector3)* outVertices.size() * 2);

	for (long a = 0; a < triangleCount; a++) {
		unsigned short i1 = indices[a * 3];
		unsigned short i2 = indices[a * 3 + 1];
		unsigned short i3 = indices[a * 3 + 2];

		Vector3 v1 = outVertices[i1].position;
		Vector3 v2 = outVertices[i2].position;
		Vector3 v3 = outVertices[i3].position;

		Vector2 w1 = outVertices[i1].texCoord;
		Vector2 w2 = outVertices[i2].texCoord;
		Vector2 w3 = outVertices[i3].texCoord;

		Vector3 e1 = v2 - v1;
		Vector3 e2 = v3 - v1;

		Vector2 tex1 = w2 - w1;
		Vector2 tex2 = w3 - w1;

		float r = 1.0f / (tex1.x * tex2.y - tex2.x * tex1.y);
		Vector3 sdir;
		sdir.x = (tex2.y * e1.x - tex1.y * e2.x) * r;
		sdir.y = (tex2.y * e1.y - tex1.y * e2.y) * r;
		sdir.z = (tex2.y * e1.z - tex1.y * e2.z) * r;

		Vector3 tdir;
		tdir.x = (tex1.x * e2.x - tex2.x * e1.x) * r;
		tdir.y = (tex1.x * e2.y - tex2.x * e1.y) * r;
		tdir.z = (tex1.x * e2.z - tex2.x * e1.z) * r;

		tan1[i1] = tan1[i1] + sdir;
		tan1[i2] = tan1[i2] + sdir;
		tan1[i3] = tan1[i3] + sdir;

		tan2[i1] = tan2[i1] + tdir;
		tan2[i2] = tan2[i2] + tdir;
		tan2[i3] = tan2[i3] + tdir;
	}

	for (int i = 0; i < outVertices.size(); i++) {
		Vector3 n = outVertices[i].normal;
		Vector3 t = tan1[i];

		Vector3 tan = (t - n * n.Dot(t)).Normalize();
		float w = (n.Cross(t).Dot(tan2[i])) < 0.0f ? -1.0f : 1.0f;
		outVertices[i].tangent = Vector4(tan.x, tan.y, tan.z, w);

		/*Vector3 tangent;
		Vector3 c1 = VectorCross(outVertices[i].normal, Vector3(0.0f, 0.0f, 1.0f));
		Vector3 c2 = VectorCross(outVertices[i].normal, Vector3(0.0f, 1.0f, 0.0f));
		if (VectorLength(c1) > VectorLength(c2)) {
		tangent = c1;
		} else {
		tangent = c2;
		}
		outVertices[i].tangent = tangent;*/
	}

	delete[] tan1;


	GraphicsDevice* pDevice = g_pGraphics->GetDevice();

	MakeBuffer(pDevice, m_pVertexBuffer, false, D3D11_BIND_VERTEX_BUFFER,
		&outVertices[0], outVertices.size() * sizeof(Vertex));
	MakeBuffer(pDevice, m_pIndexBuffer, false, D3D11_BIND_INDEX_BUFFER,
		&indices[0], indices.size() * sizeof(unsigned short));

	/*D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(Vertex)* outVertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &outVertices[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer);

	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(unsigned short)* indices.size();
	
	data.pSysMem = &indices[0];

	pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);*/
}

Mesh* Model::Begin() {
	TEMP_CONTEXT(pContext);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    m_index = 0;
    return &m_meshes[m_index];
}

Mesh* Model::Next() {
    m_index++;
    
    if (m_index != m_meshes.size()) {
        return &m_meshes[m_index];
    }
    
    return NULL;
}

void Model::Render() {
	TEMP_CONTEXT(pContext);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	MeshList::iterator it;
	for (it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		it->Render();
	}
}

void Model::Release() {
	DX_SAFE_RELEASE(m_pVertexBuffer);
	DX_SAFE_RELEASE(m_pIndexBuffer);
}

void Model::Copy(IResource *destination) {
	Model* dest = static_cast<Model*>(destination);
	dest->m_meshes = MeshList(m_meshes);
	dest->m_pIndexBuffer = m_pIndexBuffer;
	m_pIndexBuffer->AddRef();
	dest->m_pVertexBuffer = m_pVertexBuffer;
	m_pVertexBuffer->AddRef();
}