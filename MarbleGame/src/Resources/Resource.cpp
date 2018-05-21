//
//  Resource.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include <vector>

#include "Resource.h"
#include "ResourceManager.h"
#include "Image.h"
#include "../DXShader.h"
#include "Util.h"

void Texture2D::Release() {
	DX_SAFE_RELEASE(m_pView);
	DX_SAFE_RELEASE(m_pTexture);
}

void Texture2D::LoadResource(const char *path) {
	TEMP_DEVICE(pDevice);

	Image* img;
	LoadImageFromFile(path, &img, true);

	m_width = img->width;
	m_height = img->height;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = img->width;
	desc.Height = img->height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = img->imageData;
	data.SysMemPitch = img->width * 4;
	data.SysMemSlicePitch = 0;

	HRESULT res = pDevice->CreateTexture2D(&desc, &data, &m_pTexture);
	res = pDevice->CreateShaderResourceView(m_pTexture, NULL, &m_pView);

	img->Release();
}

void Texture2DArray::Release() {
	DX_SAFE_RELEASE(m_pView);
	DX_SAFE_RELEASE(m_pTexture);
}

void Texture2DArray::SetTextureNames(const char** paths, int numTextures) {
	if (m_pTextureNames) {
		delete[] m_pTextureNames;
		m_pTextureNames = nullptr;
	}

	m_pTextureNames = DBG_NEW const char*[numTextures];
	memcpy(m_pTextureNames, paths, numTextures * sizeof(const char*));
	m_numTextures = numTextures;
}

void Texture2DArray::LoadResource(const char* path) {
	TEMP_DEVICE(pDevice);
	TEMP_CONTEXT(pContext);

	int index = 0;
	Image* img;
	LoadImageFromFile((ResourceManager::GetResourcesPath() + m_pTextureNames[index]).c_str(), &img, true);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = img->width;
	texDesc.Height = img->height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = m_numTextures;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = img->imageData;
	data.SysMemPitch = img->width * 4;
	data.SysMemSlicePitch = 0;

	pDevice->CreateTexture2D(&texDesc, nullptr, &m_pTexture);
	pContext->UpdateSubresource(m_pTexture, index, nullptr, img->imageData, img->width * 4, 0);
	index++;
	img->Release();

	for (; index < m_numTextures; index++) {
		LoadImageFromFile((ResourceManager::GetResourcesPath() + m_pTextureNames[index]).c_str(), &img, true);
		pContext->UpdateSubresource(m_pTexture, index, nullptr, img->imageData, img->width * 4, 0);
		img->Release();
	}

	pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pView);

	if (m_pTextureNames) {
		delete[] m_pTextureNames;
		m_pTextureNames = nullptr;
	}
}
