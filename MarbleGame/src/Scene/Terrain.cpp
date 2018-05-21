//
//  Terrain.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 01.03.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include "Terrain.h"
#include "Scene.h"
#include "Camera.h"
#include "Resources/ResourceManager.h"
#include "../main.h"
#include "DXShader.h"

#include "Maths/VectorMath.h"

struct TerrainVertex {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
};

#ifndef EDITOR
#define Load QueueContent
#else
#define Load LoadContent
#endif

#define WAVES_RES 1024

void WaterNode::init() {
    wavesTexture = g_pResourceManager->Load<Texture2D>("waterbump.png");
    
	TEMP_DEVICE(pDevice);

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = SCREEN_WIDTH;
	desc.Height = SCREEN_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.MiscFlags = 0;

	ID3D11Texture2D* refrTex;
	HRESULT res = pDevice->CreateTexture2D(&desc, NULL, &refrTex);
	res = pDevice->CreateRenderTargetView(refrTex, NULL, &m_pRefractionView);
	res = pDevice->CreateShaderResourceView(refrTex, NULL, &m_pRefractionResource);
	refrTex->Release();

	ID3D11Texture2D* reflTex;
	res = pDevice->CreateTexture2D(&desc, NULL, &reflTex);
	res = pDevice->CreateRenderTargetView(reflTex, NULL, &m_pReflectionView);
	res = pDevice->CreateShaderResourceView(reflTex, NULL, &m_pReflectionResource);
	reflTex->Release();

	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthTex;
	res = pDevice->CreateTexture2D(&desc, NULL, &depthTex);
	res = pDevice->CreateDepthStencilView(depthTex, NULL, &m_pDepthBuffer);
	depthTex->Release();


	//waves normal map 
	/*desc.Width = WAVES_RES;
	desc.Height = WAVES_RES;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	ID3D11Texture2D* wavesTex;
	res = g_pDevice->CreateTexture2D(&desc, NULL, &wavesTex);
	res = g_pDevice->CreateRenderTargetView(wavesTex, NULL, &m_pWavesView);
	res = g_pDevice->CreateShaderResourceView(wavesTex, NULL, &m_pWavesResource);
	wavesTex->Release();*/



	WaterVertex vertices[12];
	float scale = 0.5f;
	vertices[0].position = Vector3(-125 * scale, 2, 125 * scale);
	vertices[2].position = Vector3(-125 * scale, 2, -125 * scale);
	vertices[1].position = Vector3(125 * scale, 2, -125 * scale);

	vertices[3].position = Vector3(-125 * scale, 2, 125 * scale);
	vertices[5].position = Vector3(125 * scale, 2, -125 * scale);
	vertices[4].position = Vector3(125 * scale, 2, 125 * scale);

	vertices[0].texCoord = Vector2(0.0f, 0.0f);
	vertices[2].texCoord = Vector2(0.0f, 1.0f);
	vertices[1].texCoord = Vector2(1.0f, 1.0f);

	vertices[3].texCoord = Vector2(0.0f, 0.0f);
	vertices[5].texCoord = Vector2(1.0f, 1.0f);
	vertices[4].texCoord = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 6; i++) {
		m_vAABBMin = m_vAABBMin.Min(vertices[i].position);
		m_vAABBMax = m_vAABBMax.Max(vertices[i].position);
	}

	float temp = WAVES_RES / 2.0f;
	vertices[6].position = Vector3(-temp, -temp, 0.0f);
	vertices[7].position = Vector3(-temp, temp, 0.0f);
	vertices[8].position = Vector3(temp, temp, 0.0f);

	vertices[9].position = Vector3(temp, temp, 0.0f);
	vertices[10].position = Vector3(temp, -temp, 0.0f);
	vertices[11].position = Vector3(-temp, -temp, 0.0f);

	vertices[6].texCoord = Vector2(0.0f, 0.0f);
	vertices[7].texCoord = Vector2(0.0f, 1.0f);
	vertices[8].texCoord = Vector2(1.0f, 1.0f);

	vertices[9].texCoord = Vector2(1.0f, 1.0f);
	vertices[10].texCoord = Vector2(1.0f, 0.0f);
	vertices[11].texCoord = Vector2(0.0f, 0.0f);

	D3D11_BUFFER_DESC buffDesc;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = 12 * sizeof(WaterVertex);
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;
	buffDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	res = pDevice->CreateBuffer(&buffDesc, &data, &m_pVertexBuffer);


	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = false;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.MultisampleEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	pDevice->CreateRasterizerState(&rsDesc, &m_pRSFrontCulling);
}

void WaterNode::OnRelease() {
	DX_SAFE_RELEASE(m_pRSFrontCulling);
	DX_SAFE_RELEASE(m_pRefractionView);
	DX_SAFE_RELEASE(m_pRefractionResource);
	DX_SAFE_RELEASE(m_pReflectionView);
	DX_SAFE_RELEASE(m_pReflectionResource);
	DX_SAFE_RELEASE(m_pDepthBuffer);
	DX_SAFE_RELEASE(m_pVertexBuffer);
}

bool firstPass = false;

void WaterNode::OnAdded() {
	float waterHeight = GetAbsPosition().y + 2.0f;
	m_pScene->GetRenderData()->fWaterHeight = waterHeight;
	m_pScene->UpdateRenderData();

	m_pShader = EffectManager->LoadEffect("WaterShader", TexCoord2D, false, false);
}

void WaterNode::OnRender() {
    if (!firstPass) {
        firstPass = true;

		TEMP_CONTEXT(pContext);
		
		//waves normal map rendering
		/* D3D11_VIEWPORT view;
		view.TopLeftX = 0;
		view.TopLeftY = 0;
		view.Width = WAVES_RES;
		view.Height = WAVES_RES;
		view.MinDepth = 0.0f;
		view.MaxDepth = 1.0f;

		g_pDevice->RSSetViewports(1, &view);
		g_pDevice->OMSetRenderTargets(1, &m_pWavesView, NULL);
		Vector4 color(0.5f, 0.5f, 1.0f, 0.0f);
		g_pDevice->ClearRenderTargetView(m_pWavesView, color.v);
		UINT stride = sizeof(WaterVertex);
		UINT offset = 0;
		g_pDevice->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		m_pWavesShader->SetVariable("wavesPosition", &wavesPosition);
		m_pWavesShader->Apply();
		g_pDevice->Draw(6, 6);*/

		//scene refraction + reflection
		Vector4 color = Vector4(0.0f);

		//view.Width = SCREEN_WIDTH;
		//view.Height = SCREEN_HEIGHT;
		//g_pDevice->RSSetViewports(1, &view);
		pContext->ClearRenderTargetView(m_pReflectionView, color.v);
		pContext->ClearDepthStencilView(m_pDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_pScene->GetRenderData()->bMirror = true;
		m_pScene->GetRenderData()->vClipPlane = Vector4(0.0f, -1.0f, 0.0f, GetAbsPosition().y + 2.0f + 0.05f);
		m_pScene->UpdateRenderData();

		pContext->RSSetState(m_pRSFrontCulling);
		m_pScene->Draw(m_pReflectionView, m_pDepthBuffer);


		pContext->ClearRenderTargetView(m_pRefractionView, color.v);
		pContext->ClearDepthStencilView(m_pDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_pScene->GetRenderData()->bMirror = false;
		m_pScene->UpdateRenderData();

		g_pGraphics->SetDefaultRSState();
		m_pScene->Draw(m_pRefractionView, m_pDepthBuffer);



		//water surface rendering
		m_pScene->GetRenderData()->vClipPlane = Vector4(0.0f);
		m_pScene->UpdateRenderData();

		m_pScene->GetObjectData()->mModel = GetAbsTransform().Transpose();
		m_pScene->GetObjectData()->vColor = GetColor();
		m_pScene->UpdateObjectData();

		g_pGraphics->SetBackBuffer();

		UINT stride = sizeof(WaterVertex);
		UINT offset = 0;

		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);


		ID3D11ShaderResourceView* resources[3] = {
			m_pRefractionResource, m_pReflectionResource, wavesTexture->m_pView
		};
		
		pContext->PSSetShaderResources(0, 3, resources);
		EffectManager->ApplyEffect(m_pShader);
		pContext->Draw(6, 0);

		firstPass = false;
    }
}

#include <stdint.h>

void TerrainNode::init() {
	TEMP_DEVICE(pDevice);

    texture = g_pResourceManager->Load<Texture2D>("Tileable stone texture.png");
    
    SetOrigin(VectorDown * 50.0f);
	SetOrientation(Quaternion(0, 0, 0, 1));
    
	m_vAABBMin = Vector3(FLT_MAX);
	m_vAABBMax = Vector3(-FLT_MAX);
	std::string terrainPath = ResourceManager::GetResourcesPath() + "Terrain.dat";
    FILE* stream = fopen(terrainPath.c_str(), "rb");
    
    int blockSize;
    fread(&blockSize, sizeof(int), 1, stream);
    TerrainVertex* vertexArray = new TerrainVertex[blockSize];
    fread(vertexArray, sizeof(TerrainVertex), blockSize, stream);
    
    for (int i = 0; i < blockSize; i++) {
        vertexArray[i].position *= 0.5f;
		Vector3 tempNormal = vertexArray[i].normal;
		Vector2 tempTexCoord = vertexArray[i].texCoord;
		vertexArray[i].normal = Vector3(tempTexCoord.x, tempTexCoord.y, tempNormal.x);
		vertexArray[i].texCoord = Vector2(tempNormal.y, tempNormal.z);
		m_vAABBMin = m_vAABBMin.Min(vertexArray[i].position);
		m_vAABBMax = m_vAABBMax.Max(vertexArray[i].position);
    }
    
    fread(&indexBufferSize, sizeof(unsigned int), 1, stream);
    uint16_t* indices = new uint16_t[indexBufferSize];
    fread(indices, sizeof(uint16_t), indexBufferSize, stream);    
    
    fclose(stream);

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = blockSize * sizeof(TerrainVertex);
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexArray;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT res = pDevice->CreateBuffer(&bufDesc, &data, &m_pVertexBuffer);

	bufDesc.ByteWidth = indexBufferSize * sizeof(unsigned short);
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data.pSysMem = indices;


	res = pDevice->CreateBuffer(&bufDesc, &data, &m_pIndexBuffer);


	delete[] vertexArray;
	delete[] indices;

}

void TerrainNode::OnRelease() {
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
}

void TerrainNode::OnRender() {
	GraphicsContext* pContext = g_pGraphics->GetContext();

	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pContext->PSSetShaderResources(0, 1, &texture->m_pView);
	EffectManager->ApplyEffect(m_pShader);
	pContext->DrawIndexed(indexBufferSize, 0, 0);

    //renderSubNodes();
}

void TerrainNode::OnAdded() {
	m_pShader = EffectManager->LoadEffect("TerrainShader", TexCoord2D, true, false);
}