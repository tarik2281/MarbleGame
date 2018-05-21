#include "Platform.h"

#include <vector>

#include "Resources/Model.h"
#include "Resources/MaterialList.h"
#include "Scene/Scene.h"
#include "DXShader.h"
#include "Main.h"

#define TEXTILESIZE 7.5f

#ifndef _WINDLL
#define BUFFER_TYPE D3D11_USAGE_DEFAULT
#define BUFFER_ACCESS 0
#else
#define BUFFER_TYPE D3D11_USAGE_DYNAMIC
#define BUFFER_ACCESS D3D11_CPU_ACCESS_WRITE
#endif

#define EFFECT_NAME "Shader"

#ifndef _WINDLL
PlatformInstances::PlatformInstances(Material mat) : _Base() {
	m_mat = mat;
}

void PlatformInstances::InitData(int size) {
	Vertex* vertices = DBG_NEW Vertex[m_instances.size() * NUM_MAX_PLATFORM_VERTICES];
	uint16_t* indices = DBG_NEW uint16_t[m_instances.size() * NUM_MAX_PLATFORM_INDICES];

	InstanceList::iterator it = m_instances.begin();
	int numVertices = 0;
	m_numIndices = 0;
	for (; it != m_instances.end(); ++it) {
		PlatformNode* node = (*it);
		node->MakeVertices(vertices + numVertices, &(indices[m_numIndices]));
		int nVertices = node->GetNumVertices();
		int nIndices = node->GetNumIndices();
		Quaternion rot = node->GetOrientation();
		Vector3 orig = node->GetOrigin();

		for (int i = 0; i < nVertices; i++) {
			Vertex& v = vertices[numVertices + i];
			v.position = rot * v.position + orig;
			v.normal = rot * v.normal;

			Vector3 temp(v.tangent.x, v.tangent.y, v.tangent.z);
			v.tangent = Vector4(rot * temp, v.tangent.w);

			m_vAABBMin = m_vAABBMin.Min(v.position);
			m_vAABBMax = m_vAABBMax.Max(v.position);
		}

		for (int i = 0; i < nIndices; i++) {
			indices[m_numIndices + i] += numVertices;
		}

		numVertices += nVertices;
		m_numIndices += nIndices;
	}


	TEMP_DEVICE(pDevice);

	MakeBuffer(pDevice, m_instanceBuffer, false, D3D11_BIND_VERTEX_BUFFER, vertices, numVertices * sizeof(Vertex));
	MakeBuffer(pDevice, m_indexBuffer, false, D3D11_BIND_INDEX_BUFFER, indices, m_numIndices * sizeof(unsigned short));

	delete[] indices;
	delete[] vertices;
}

void PlatformInstances::OnAdded() {
	m_pEffect = EffectManager->LoadEffect(EFFECT_NAME, TexCoord2D, true, true);
}

void PlatformInstances::OnRender() {
	TEMP_CONTEXT(pContext);

	if (m_mat.diffuseMap && m_mat.normalMap) {
		ID3D11ShaderResourceView* resources[2] = {
			m_mat.diffuseMap->m_pView, m_mat.normalMap->m_pView
		};
		pContext->PSSetShaderResources(0, 2, resources);
	}

	EffectManager->ApplyEffect(m_pEffect);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_instanceBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->DrawIndexed(m_numIndices, 0, 0);
}

void PlatformInstances::OnRelease() {
	_Base::OnRelease();
	m_indexBuffer->Release();
}
#endif

Vector3 PlatformData::GetPosition() const {
	float xPosition;
	float yPosition = bb.halfSize.y * TILE_SCALING / 2.0f + bb.origin.y * TILE_SCALING;
	float zPosition;

	if (rotation == West || rotation == East) {
		xPosition = bb.halfSize.z * TILE_SCALING / 2.0f + bb.origin.x * TILE_SCALING;
		zPosition = bb.halfSize.x * TILE_SCALING / 2.0f + bb.origin.z * TILE_SCALING;
	}
	else {
		xPosition = bb.halfSize.x * TILE_SCALING / 2.0f + bb.origin.x * TILE_SCALING;
		zPosition = bb.halfSize.z * TILE_SCALING / 2.0f + bb.origin.z * TILE_SCALING;
	}

	return Vector3(xPosition, yPosition, zPosition);
}

void PlatformNode::Initialize() {
#ifdef _WINDLL
	TEMP_DEVICE(pDevice);

	typedef unsigned short USHORT;

	Vertex vertices[NUM_MAX_PLATFORM_VERTICES];
	USHORT indices[NUM_MAX_PLATFORM_INDICES];

	ZeroMemory(vertices, NUM_MAX_PLATFORM_VERTICES * sizeof(Vertex));
	ZeroMemory(indices, NUM_MAX_PLATFORM_INDICES * sizeof(USHORT));

	this->MakeVertices(vertices, indices);

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = GetNumVertices() * sizeof(Vertex);
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = BUFFER_ACCESS;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = BUFFER_TYPE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	pDevice->CreateBuffer(&bufDesc, &data, &m_pVertexBuffer);

	bufDesc.CPUAccessFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = GetNumIndices() * sizeof(unsigned short);
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data.pSysMem = indices;

	pDevice->CreateBuffer(&bufDesc, &data, &m_pIndexBuffer);
	m_indicesSize = GetNumIndices();
#endif
}

void PlatformNode::OnRelease() {
#ifdef _WINDLL
	DX_SAFE_RELEASE(m_pVertexBuffer);
	DX_SAFE_RELEASE(m_pIndexBuffer);
#endif
}

void PlatformNode::OnRender() {
#ifdef _WINDLL
	TEMP_CONTEXT(pContext);

	Shader* shader = m_pScene->UseShader();
	if (mat.diffuseMap)
		shader->SetVariable("someTexture", mat.diffuseMap->m_pView);
	if (mat.normalMap)
		shader->SetVariable("normalMap", mat.normalMap->m_pView);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	shader->Apply();
	pContext->DrawIndexed(m_indicesSize, 0, 0);

	//renderSubNodes();
#endif
}

void PlatformNode::MakeVertices(Vertex* vertices, uint16_t* indices) {
#define RNG_CMP(index, begin, end) (index >= begin && index <= end)
	/*#define CMP_FRONT(index) RNG_CMP(index, 0, 3)
	#define CMP_BACK(index) RNG_CMP(index, 4, 7)
	#define CMP_LEFT(index) RNG_CMP(index, 8, 11)
	#define CMP_RIGHT(index) RNG_CMP(index, 12, 15)
	#define CMP_TOP(index) RNG_CMP(index, 16, 19)
	#define CMP_BOTTOM(index) RNG_CMP(index, 20, 23)*/
	// front: 0-3;  back: 4-7;  left: 8-11;  right: 12-15;  top: 16-19;  bottom: 20-23;
	float xTexCoord = bb.halfSize.x * TILE_SCALING / TEXTILESIZE;
	float yTexCoord = bb.halfSize.y * TILE_SCALING / TEXTILESIZE;
	float zTexCoord = bb.halfSize.z * TILE_SCALING / TEXTILESIZE;

	Vector3 halfEx = bb.halfSize / 2.0f * TILE_SCALING;

	vertices[0].texCoord = Vector2(0.0f, 0.0f);				vertices[0].position = Vector3(-halfEx.x, -halfEx.y, +halfEx.z);
	vertices[1].texCoord = Vector2(xTexCoord, 0.0f);		vertices[1].position = Vector3(+halfEx.x, -halfEx.y, +halfEx.z);
	vertices[2].texCoord = Vector2(xTexCoord, yTexCoord);	vertices[2].position = Vector3(+halfEx.x, +halfEx.y, +halfEx.z);
	vertices[3].texCoord = Vector2(0.0f, yTexCoord);		vertices[3].position = Vector3(-halfEx.x, +halfEx.y, +halfEx.z);

	vertices[4].texCoord = Vector2(0.0f, 0.0f);				vertices[4].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[5].texCoord = Vector2(xTexCoord, 0.0f);		vertices[5].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[6].texCoord = Vector2(xTexCoord, yTexCoord);	vertices[6].position = Vector3(-halfEx.x, +halfEx.y, -halfEx.z);
	vertices[7].texCoord = Vector2(0.0f, yTexCoord);		vertices[7].position = Vector3(+halfEx.x, +halfEx.y, -halfEx.z);

	vertices[8].texCoord = Vector2(0.0f, 0.0f);				vertices[8].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[9].texCoord = Vector2(zTexCoord, 0.0f);		vertices[9].position = Vector3(-halfEx.x, -halfEx.y, +halfEx.z);
	vertices[10].texCoord = Vector2(zTexCoord, yTexCoord);	vertices[10].position = Vector3(-halfEx.x, +halfEx.y, +halfEx.z);
	vertices[11].texCoord = Vector2(0.0f, yTexCoord);		vertices[11].position = Vector3(-halfEx.x, +halfEx.y, -halfEx.z);

	vertices[12].texCoord = Vector2(0.0f, 0.0f);			vertices[12].position = Vector3(+halfEx.x, -halfEx.y, +halfEx.z);
	vertices[13].texCoord = Vector2(zTexCoord, 0.0f);		vertices[13].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[14].texCoord = Vector2(zTexCoord, yTexCoord);	vertices[14].position = Vector3(+halfEx.x, +halfEx.y, -halfEx.z);
	vertices[15].texCoord = Vector2(0.0f, yTexCoord);		vertices[15].position = Vector3(+halfEx.x, +halfEx.y, +halfEx.z);

	vertices[16].texCoord = Vector2(0.0f, 0.0f);			vertices[16].position = Vector3(-halfEx.x, +halfEx.y, +halfEx.z);
	vertices[17].texCoord = Vector2(xTexCoord, 0.0f);		vertices[17].position = Vector3(+halfEx.x, +halfEx.y, +halfEx.z);
	vertices[18].texCoord = Vector2(xTexCoord, zTexCoord);	vertices[18].position = Vector3(+halfEx.x, +halfEx.y, -halfEx.z);
	vertices[19].texCoord = Vector2(0.0f, zTexCoord);		vertices[19].position = Vector3(-halfEx.x, +halfEx.y, -halfEx.z);

	vertices[20].texCoord = Vector2(xTexCoord, zTexCoord);	vertices[20].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[21].texCoord = Vector2(0.0f, zTexCoord);		vertices[21].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[22].texCoord = Vector2(0.0f, 0.0f);			vertices[22].position = Vector3(+halfEx.x, -halfEx.y, +halfEx.z);
	vertices[23].texCoord = Vector2(xTexCoord, 0.0f);		vertices[23].position = Vector3(-halfEx.x, -halfEx.y, +halfEx.z);


	for (int i = 0; i < 24; i++) {
		if (RNG_CMP(i, 0, 3)) {
			vertices[i].normal = Vector3(0.0f, 0.0f, 1.0f);
			vertices[i].tangent = Vector4(1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 4, 7)) {
			vertices[i].normal = Vector3(0.0f, 0.0f, -1.0f);
			vertices[i].tangent = Vector4(-1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 8, 11)) {
			vertices[i].normal = Vector3(-1.0f, 0.0f, 0.0f);
			vertices[i].tangent = Vector4(0.0f, 0.0f, 1.0f, -1.0f);
		}
		else if (RNG_CMP(i, 12, 15)) {
			vertices[i].normal = Vector3(1.0f, 0.0f, 0.0f);
			vertices[i].tangent = Vector4(0.0f, 0.0f, -1.0f, 1.0f);
		}
		else if (RNG_CMP(i, 16, 19)) {
			vertices[i].normal = Vector3(0.0f, 1.0f, 0.0f);
			vertices[i].tangent = Vector4(1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 20, 23)) {
			vertices[i].normal = Vector3(0.0f, -1.0f, 0.0f);
			vertices[i].tangent = Vector4(-1.0f, 0.0f, 0.0f, 1.0f);
		}
	}
#undef RNG_CMP

	if (indices) {
		typedef unsigned short USHORT;

		USHORT tempIndices[6 * 2 * 3] = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		};

		memcpy(indices, tempIndices, 6 * 2 * 3 * sizeof(USHORT));
	}
}

void PlatformNode::Resize() {
#ifdef _WINDLL
	TEMP_CONTEXT(pContext);

	D3D11_MAPPED_SUBRESOURCE data;
	Vertex* vertices;
	HRESULT res = pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	vertices = (Vertex*)data.pData;
	MakeVertices(vertices, nullptr);

	pContext->Unmap(m_pVertexBuffer, 0);
#endif
}

void RampNode::MakeVertices(Vertex* vertices, uint16_t* indices) {
#define RNG_CMP(index, begin, end) index >= begin && index <= end
	float xTexCoord = bb.halfSize.x * TILE_SCALING / TEXTILESIZE;
	float yTexCoord = bb.halfSize.y * TILE_SCALING / TEXTILESIZE;
	float zTexCoord = bb.halfSize.z * TILE_SCALING / TEXTILESIZE;

	Vector3 halfEx = bb.halfSize / 2.0f * TILE_SCALING;
	float rampTexCoord = sqrtf((bb.halfSize.x * TILE_SCALING * bb.halfSize.x * TILE_SCALING) + (bb.halfSize.y * TILE_SCALING * bb.halfSize.y * TILE_SCALING)) / TEXTILESIZE;

	vertices[0].texCoord = Vector2(0.0f, 0.0f);				vertices[0].position = Vector3(-halfEx.x, -halfEx.y, halfEx.z);
	vertices[1].texCoord = Vector2(xTexCoord, 0.0f);		vertices[1].position = Vector3(halfEx.x, -halfEx.y, halfEx.z);
	vertices[2].texCoord = Vector2(0.0f, yTexCoord);		vertices[2].position = Vector3(-halfEx.x, halfEx.y, halfEx.z);

	vertices[3].texCoord = Vector2(0.0f, 0.0f);				vertices[3].position = Vector3(halfEx.x, -halfEx.y, -halfEx.z);
	vertices[4].texCoord = Vector2(xTexCoord, 0.0f);		vertices[4].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[5].texCoord = Vector2(xTexCoord, yTexCoord);	vertices[5].position = Vector3(-halfEx.x, halfEx.y, -halfEx.z);

	vertices[6].texCoord = Vector2(0.0f, 0.0f);				vertices[6].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[7].texCoord = Vector2(zTexCoord, 0.0f);		vertices[7].position = Vector3(-halfEx.x, -halfEx.y, halfEx.z);
	vertices[8].texCoord = Vector2(zTexCoord, yTexCoord);	vertices[8].position = Vector3(-halfEx.x, halfEx.y, halfEx.z);
	vertices[9].texCoord = Vector2(0.0f, yTexCoord);		vertices[9].position = Vector3(-halfEx.x, halfEx.y, -halfEx.z);

	vertices[10].texCoord = Vector2(xTexCoord, zTexCoord);	vertices[10].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[11].texCoord = Vector2(0.0f, zTexCoord);		vertices[11].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[12].texCoord = Vector2(0.0f, 0.0f);			vertices[12].position = Vector3(+halfEx.x, -halfEx.y, +halfEx.z);
	vertices[13].texCoord = Vector2(xTexCoord, 0.0f);		vertices[13].position = Vector3(-halfEx.x, -halfEx.y, +halfEx.z);

	vertices[14].texCoord = Vector2(0.0f, 0.0f);			vertices[14].position = Vector3(-halfEx.x, +halfEx.y, halfEx.z);
	vertices[15].texCoord = Vector2(rampTexCoord, 0.0f);		vertices[15].position = Vector3(+halfEx.x, -halfEx.y, halfEx.z);
	vertices[16].texCoord = Vector2(rampTexCoord, zTexCoord);	vertices[16].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[17].texCoord = Vector2(0.0f, zTexCoord);		vertices[17].position = Vector3(-halfEx.x, +halfEx.y, -halfEx.z);

	Vector3 e1 = vertices[15].position - vertices[14].position;
	Vector3 e2 = vertices[17].position - vertices[14].position;

	Vector3 normal = e1.Cross(e2).Normalize();
	Vector3 tangent = e1.Normalize();

	for (int i = 0; i < 18; i++) {
		if (RNG_CMP(i, 0, 2)) {
			vertices[i].normal = Vector3(0.0f, 0.0f, 1.0f);
			vertices[i].tangent = Vector4(1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 3, 5)) {
			vertices[i].normal = Vector3(0.0f, 0.0f, -1.0f);
			vertices[i].tangent = Vector4(-1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 6, 9)) {
			vertices[i].normal = Vector3(-1.0f, 0.0f, 0.0f);
			vertices[i].tangent = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		else if (RNG_CMP(i, 10, 13)) {
			vertices[i].normal = Vector3(0.0f, -1.0f, 0.0f);
			vertices[i].tangent = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else if (RNG_CMP(i, 14, 17)) {
			vertices[i].normal = normal;
			vertices[i].tangent = Vector4(tangent.x, tangent.y, tangent.z, -1.0f);
		}
	}

	if (indices) {
		typedef unsigned short USHORT;

		USHORT tempIndices[2 * 3 + 3 * 3 * 2] = {
			0, 1, 2,
			3, 4, 5,
			6, 7, 8, 8, 9, 6,
			10, 11, 12, 12, 13, 10,
			14, 15, 16, 16, 17, 14
		};

		memcpy(indices, tempIndices, GetNumIndices() * sizeof(USHORT));
	}
#undef RNG_CMP
}

void CornerNode::MakeVertices(Vertex* vertices, uint16_t* indices) {
#define RNG_CMP(index, begin, end) index >= begin && index <= end

	float xTexCoord = bb.halfSize.x * TILE_SCALING / TEXTILESIZE;
	float yTexCoord = bb.halfSize.z * TILE_SCALING / TEXTILESIZE;
	float zTexCoord = bb.halfSize.y * TILE_SCALING / TEXTILESIZE;

	Vector3 halfEx = bb.halfSize / 2.0f * TILE_SCALING;
	float rampTexCoord = sqrtf((bb.halfSize.x * TILE_SCALING * bb.halfSize.x * TILE_SCALING) + (bb.halfSize.z * TILE_SCALING * bb.halfSize.z * TILE_SCALING)) / TEXTILESIZE;

	vertices[0].texCoord = Vector2(0.0f, 0.0f);				vertices[0].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[1].texCoord = Vector2(xTexCoord, 0.0f);		vertices[1].position = Vector3(halfEx.x, -halfEx.y, -halfEx.z);
	vertices[2].texCoord = Vector2(0.0f, yTexCoord);		vertices[2].position = Vector3(-halfEx.x, -halfEx.y, halfEx.z);

	vertices[3].texCoord = Vector2(0.0f, 0.0f);				vertices[3].position = Vector3(halfEx.x, halfEx.y, -halfEx.z);
	vertices[4].texCoord = Vector2(xTexCoord, 0.0f);		vertices[4].position = Vector3(-halfEx.x, halfEx.y, -halfEx.z);
	vertices[5].texCoord = Vector2(xTexCoord, yTexCoord);	vertices[5].position = Vector3(-halfEx.x, halfEx.y, halfEx.z);

	vertices[6].texCoord = Vector2(0.0f, 0.0f);				vertices[6].position = Vector3(-halfEx.x, halfEx.y, -halfEx.z);
	vertices[7].texCoord = Vector2(0.0f, zTexCoord);		vertices[7].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	vertices[8].texCoord = Vector2(yTexCoord, zTexCoord);	vertices[8].position = Vector3(-halfEx.x, -halfEx.y, halfEx.z);
	vertices[9].texCoord = Vector2(yTexCoord, 0.0f);		vertices[9].position = Vector3(-halfEx.x, halfEx.y, halfEx.z);

	vertices[10].texCoord = Vector2(xTexCoord, zTexCoord);	vertices[10].position = Vector3(-halfEx.x, halfEx.y, -halfEx.z);
	vertices[11].texCoord = Vector2(0.0f, zTexCoord);		vertices[11].position = Vector3(+halfEx.x, halfEx.y, -halfEx.z);
	vertices[12].texCoord = Vector2(0.0f, 0.0f);			vertices[12].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[13].texCoord = Vector2(xTexCoord, 0.0f);		vertices[13].position = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);

	vertices[14].texCoord = Vector2(0.0f, 0.0f);			vertices[14].position = Vector3(-halfEx.x, -halfEx.y, halfEx.z);
	vertices[15].texCoord = Vector2(rampTexCoord, 0.0f);		vertices[15].position = Vector3(+halfEx.x, -halfEx.y, -halfEx.z);
	vertices[16].texCoord = Vector2(rampTexCoord, zTexCoord);	vertices[16].position = Vector3(+halfEx.x, +halfEx.y, -halfEx.z);
	vertices[17].texCoord = Vector2(0.0f, zTexCoord);		vertices[17].position = Vector3(-halfEx.x, +halfEx.y, +halfEx.z);

	Vector3 e1 = vertices[15].position - vertices[14].position;
	Vector3 e2 = vertices[17].position - vertices[14].position;

	Vector3 normal = e1.Cross(e2).Normalize();
	Vector3 tangent = e1.Normalize();

	for (int i = 0; i < 18; i++) {
		if (RNG_CMP(i, 0, 2)) {
			vertices[i].normal = Vector3(0.0f, -1.0f, 0.0f);
			vertices[i].tangent = Vector4(1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 3, 5)) {
			vertices[i].normal = Vector3(0.0f, 1.0f, 0.0f);
			vertices[i].tangent = Vector4(-1.0f, 0.0f, 0.0f, -1.0f);
		}
		else if (RNG_CMP(i, 6, 9)) {
			vertices[i].normal = Vector3(-1.0f, 0.0f, 0.0f);
			vertices[i].tangent = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		else if (RNG_CMP(i, 10, 13)) {
			vertices[i].normal = Vector3(0.0f, 0.0f, -1.0f);
			vertices[i].tangent = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else if (RNG_CMP(i, 14, 17)) {
			vertices[i].normal = normal;
			vertices[i].tangent = Vector4(tangent.x, tangent.y, tangent.z, 1.0f);
		}
	}

	if (indices) {
		typedef unsigned short USHORT;

		USHORT tempIndices[2 * 3 + 3 * 3 * 2] = {
			0, 1, 2,
			3, 4, 5,
			6, 7, 8, 8, 9, 6,
			10, 11, 12, 12, 13, 10,
			14, 15, 16, 16, 17, 14
		};

		memcpy(indices, tempIndices, GetNumIndices() * sizeof(USHORT));
	}
#undef RNG_CMP
}

void PlatformData::Deserialize(FILE *fStream) {
	char name[100];
	char parentName[100];

	int x, y, z;
	int w, h, d;
	int rot;

	fscanf(fStream, "%s %s %s %d %d %d  %d %d %d  %d", name, parentName, material, &x, &y, &z,
		&w, &h, &d, &rot);

	bb.origin = Vector3(x, y, z);
	bb.halfSize = Vector3(w, h, d);
	rotation = (PlatformRotation)rot;

	m_name = name;
	m_parentName = parentName;
}

void PlatformData::Serialize(FILE *fStream) const {
	fprintf(fStream, "%s %s %s %d %d %d  %d %d %d  %d", m_name.c_str(), m_parentName.c_str(), material, (int)bb.origin.x, (int)bb.origin.y, (int)bb.origin.z,
		(int)bb.halfSize.x, (int)bb.halfSize.y, (int)bb.halfSize.z, (int)rotation);
}

PhysicsShape* PlatformData::MakeShape() const {
#ifndef _WINDLL
	PhysicsShape* shape = g_pPhysics->MakeBoxShape(bb.halfSize * TILE_SCALING / 2.0f);
	return shape;
#else
	PhysicsShape* shape = g_pPhysics->MakeBoxShape(bb.halfSize * TILE_SCALING / 2.0f + 0.05f);
	return shape;
#endif
}

SceneNode* PlatformData::MakeSceneNode() const {
	Material pMat = g_pMaterials->GetVisualMaterial(material);
	PlatformNode* node = DBG_NEW PlatformNode(pMat, bb);
	node->updateTransform();
	return node;
	return nullptr;
}

PhysicsShape* RampData::MakeShape() const {
#ifndef _WINDLL
	Vector3 halfEx = bb.halfSize / 2.0f * TILE_SCALING;

	Vector3 v[6];
	v[0] = Vector3(-halfEx.x, -halfEx.y, halfEx.z);
	v[1] = Vector3(-halfEx.x, halfEx.y, halfEx.z);
	v[2] = Vector3(-halfEx.x, halfEx.y, -halfEx.z);
	v[3] = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	v[4] = Vector3(halfEx.x, -halfEx.y, -halfEx.z);
	v[5] = Vector3(halfEx.x, -halfEx.y, halfEx.z);

	PhysicsShape* shape = g_pPhysics->MakeConvexShape(v, 6);

	shape->setMargin(0.0f);
	return shape;
#else
	return g_pPhysics->MakeBoxShape(bb.halfSize * TILE_SCALING / 2.0f + 0.05f);
#endif 
}

SceneNode* RampData::MakeSceneNode() const {
	Material pMat = g_pMaterials->GetVisualMaterial(material);
	RampNode* node = DBG_NEW RampNode(pMat, bb);
	node->updateTransform();
	return node;
	return nullptr;
}

PhysicsShape* CornerData::MakeShape() const {
#ifndef _WINDLL
	Vector3 halfEx = bb.halfSize / 2.0f * TILE_SCALING;

	Vector3 v[6];
	v[0] = Vector3(-halfEx.x, -halfEx.y, -halfEx.z);
	v[1] = Vector3(-halfEx.x, -halfEx.y, halfEx.z);
	v[2] = Vector3(-halfEx.x, halfEx.y, halfEx.z);
	v[3] = Vector3(-halfEx.x, halfEx.y, -halfEx.z);
	v[4] = Vector3(halfEx.x, halfEx.y, -halfEx.z);
	v[5] = Vector3(halfEx.x, -halfEx.y, -halfEx.z);

	PhysicsShape* shape = g_pPhysics->MakeConvexShape(v, 6);

	shape->setMargin(0.0f);
	return shape;

#else
	return g_pPhysics->MakeBoxShape(bb.halfSize * TILE_SCALING / 2.0f + 0.05f);
#endif
}

SceneNode* CornerData::MakeSceneNode() const {
	Material pMat = g_pMaterials->GetVisualMaterial(material);
	CornerNode* node = DBG_NEW CornerNode(pMat, bb);
	node->updateTransform();
	return node;
}
