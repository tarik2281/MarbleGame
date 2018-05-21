//
//  Skybox.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 28.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "Skybox.h"
#include "Scene.h"
#include "Resources/Image.h"
#include "Resources/ResourceManager.h"
#include "Resources/Resource.h"
#include "DXShader.h"
#include "CGraphics.h"
#include "../main.h"
#include "Util.h"

#ifdef _WINDLL
#define Load LoadContent
#else 
#define Load QueueContent
#endif

#define SKYBOX_SHADER "SkyboxShader"

void SkyboxNode::init() {
	TEMP_DEVICE(pDevice);

    const char* textureNames[TexturesLength] = {
        "SkyboxLeft.png", "SkyboxFront.png", "SkyboxRight.png", "SkyboxBack.png",
        "SkyboxTop.png", "SkyboxBottom.png"
    };

	m_pTextures = g_pResourceManager->QueueTexture2DArray("SkyboxLeft.png", textureNames, TexturesLength);

    Vertex v[TexturesLength * 4];
    float scale = 2.0f;
    float offset = 0.0f;
    
    // Left
    v[0].position = Vector3(-SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[1].position = Vector3(-SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[2].position = Vector3(-SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[3].position = Vector3(-SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[0].texCoord = Vector3(0.0f, 0.0f, 0.0f);
    v[1].texCoord = Vector3(1.0f, 0.0f, 0.0f);
    v[2].texCoord = Vector3(1.0f, 1.0f, 0.0f);
    v[3].texCoord = Vector3(0.0f, 1.0f, 0.0f);
    
    // Front
    v[4].position = Vector3(-SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[5].position = Vector3(SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[6].position = Vector3(SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[7].position = Vector3(-SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[4].texCoord = Vector3(0.0f, 0.0f, 1.0f);
    v[5].texCoord = Vector3(1.0f, 0.0f, 1.0f);
    v[6].texCoord = Vector3(1.0f, 1.0f, 1.0f);
    v[7].texCoord = Vector3(0.0f, 1.0f, 1.0f);
    
    // Right
    v[8].position = Vector3(SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[9].position = Vector3(SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[10].position = Vector3(SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[11].position = Vector3(SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[8].texCoord = Vector3(0.0f, 0.0f, 2.0f);
	v[9].texCoord = Vector3(1.0f, 0.0f, 2.0f);
	v[10].texCoord = Vector3(1.0f, 1.0f, 2.0f);
	v[11].texCoord = Vector3(0.0f, 1.0f, 2.0f);
    
    // Back
    v[12].position = Vector3(SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[13].position = Vector3(-SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[14].position = Vector3(-SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[15].position = Vector3(SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
	v[12].texCoord = Vector3(0.0f, 0.0f, 3.0f);
	v[13].texCoord = Vector3(1.0f, 0.0f, 3.0f);
	v[14].texCoord = Vector3(1.0f, 1.0f, 3.0f);
	v[15].texCoord = Vector3(0.0f, 1.0f, 3.0f);
    
    // Top
    v[16].position = Vector3(-SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[17].position = Vector3(SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[18].position = Vector3(SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[19].position = Vector3(-SKYBOX_SIZE / scale, SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
	v[16].texCoord = Vector3(1.0f, 1.0f, 4.0f);
	v[17].texCoord = Vector3(0.0f, 1.0f, 4.0f);
	v[18].texCoord = Vector3(0.0f, 0.0f, 4.0f);
	v[19].texCoord = Vector3(1.0f, 0.0f, 4.0f);
    
    // bottom
    v[20].position = Vector3(-SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[21].position = Vector3(SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, -SKYBOX_SIZE / scale);
    v[22].position = Vector3(SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
    v[23].position = Vector3(-SKYBOX_SIZE / scale, -SKYBOX_SIZE / scale + offset, SKYBOX_SIZE / scale);
	v[20].texCoord = Vector3(1.0f, 0.0f, 5.0f);
	v[21].texCoord = Vector3(0.0f, 0.0f, 5.0f);
	v[22].texCoord = Vector3(0.0f, 1.0f, 5.0f);
	v[23].texCoord = Vector3(1.0f, 1.0f, 5.0f);
    
	unsigned short indices[6 * 3 * 2] = {
		0, 1, 2,  2, 3, 0,
		4, 5, 7,  7, 5, 6,
		8, 9, 10,  10, 11, 8,
		12, 13, 14,  14, 15, 12,
		16, 17, 18,  18, 19, 16,
		20, 22, 21,  22, 20, 23
	};

	MakeBuffer(pDevice, m_pVertexBuffer, false, D3D11_BIND_VERTEX_BUFFER, v, TexturesLength * 4 * sizeof(Vertex));
	MakeBuffer(pDevice, m_pIndexBuffer, false, D3D11_BIND_INDEX_BUFFER, indices, 36 * sizeof(unsigned short));

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState);
}

void SkyboxNode::OnRender() {
	TEMP_CONTEXT(pContext);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	pContext->OMSetDepthStencilState(m_pDSState, 0);
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	EffectManager->ApplyEffect(m_pEffect);

	pContext->PSSetShaderResources(0, 1, &m_pTextures->m_pView);
	pContext->DrawIndexed(6 * TexturesLength, 0, 0);

	g_pGraphics->SetDefaultDSState();
}

void SkyboxNode::OnRelease() {
	DX_SAFE_RELEASE(m_pVertexBuffer);
	DX_SAFE_RELEASE(m_pIndexBuffer);
	DX_SAFE_RELEASE(m_pDSState);
}

void SkyboxNode::OnAdded() {
	m_pEffect = EffectManager->LoadEffect(SKYBOX_SHADER, TexCoord3D, false, false);
}

void SkyboxNode::OnRemoved() {

}