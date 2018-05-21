
#include <stdio.h>
#include <string>
#include <vector>

#include "DXShader.h"
#include "Maths/MatrixMath.h"
#include "Maths/VectorMath.h"
#include "Resources\ResourceManager.h"
#include "main.h"

#ifdef _DEBUG
#pragma comment(lib, "d3dcompiler.lib")
#endif

std::string CEffectManager::s_shaderPath = "";

void CBaseEffect::Initialize(GraphicsDevice* pDevice, ShaderSource vs, ShaderSource ps) {
	CreateInputLayout(pDevice, vs);

	pDevice->CreateVertexShader(vs.pByteCode, vs.size, nullptr, &m_pVertexShader);
	
	if (ps.pByteCode != nullptr || ps.size != 0)
		pDevice->CreatePixelShader(ps.pByteCode, ps.size, nullptr, &m_pPixelShader);
}

void CBaseEffect::CreateInputLayout(GraphicsDevice* pDevice, ShaderSource vs) {
	int numElements = 1;
	D3D11_INPUT_ELEMENT_DESC inputElements[4];

	inputElements[0].SemanticName = "POSITION";
	inputElements[0].SemanticIndex = 0;
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[0].InputSlot = 0;
	inputElements[0].AlignedByteOffset = 0;
	inputElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElements[0].InstanceDataStepRate = 0;


	if (m_texCoordType != TexCoordNone) {
		numElements++;
		DXGI_FORMAT format = (m_texCoordType == TexCoord2D ? DXGI_FORMAT_R32G32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT);
		inputElements[numElements - 1].SemanticName = "TEXCOORD";
		inputElements[numElements - 1].SemanticIndex = 0;
		inputElements[numElements - 1].Format = format;
		inputElements[numElements - 1].InputSlot = 0;
		inputElements[numElements - 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[numElements - 1].InstanceDataStepRate = 0;

	}

	if (m_bUseNormal) {
		numElements++;
		inputElements[numElements - 1].SemanticName = "NORMAL";
		inputElements[numElements - 1].SemanticIndex = 0;
		inputElements[numElements - 1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[numElements - 1].InputSlot = 0;
		inputElements[numElements - 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[numElements - 1].InstanceDataStepRate = 0;
	}

	if (m_bUseTangent) {
		numElements++;
		inputElements[numElements - 1].SemanticName = "TANGENT";
		inputElements[numElements - 1].SemanticIndex = 0;
		inputElements[numElements - 1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElements[numElements - 1].InputSlot = 0;
		inputElements[numElements - 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[numElements - 1].InstanceDataStepRate = 0;
	}

	pDevice->CreateInputLayout(inputElements, numElements, vs.pByteCode, vs.size, &m_pInputLayout);
}

void CBaseEffect::OnRelease() {
	DX_SAFE_RELEASE(m_pInputLayout);
	DX_SAFE_RELEASE(m_pVertexShader);
	DX_SAFE_RELEASE(m_pPixelShader);
}

void CBaseEffect::OnApply(GraphicsContext* pContext, bool usePS) {
	pContext->IASetInputLayout(m_pInputLayout);

	pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	if (usePS)
		pContext->PSSetShader(m_pPixelShader, nullptr, 0);
}


void CEffectManager::Initialize() {
	TEMP_DEVICE(pDevice);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memset(samplerDesc.BorderColor, 0, 4);
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerClamp);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerWrap);

	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerShadow);
}

void CEffectManager::Release() {
	EffectList::iterator it = m_effects.begin();
	for (; it != m_effects.end(); ++it) {
		CBaseEffect* effect = it->second;
		effect->OnRelease();
		delete effect;
	}

	m_effects.clear();

	DX_SAFE_RELEASE(m_pSamplerClamp);
	DX_SAFE_RELEASE(m_pSamplerWrap);
	DX_SAFE_RELEASE(m_pSamplerShadow);
}

void CEffectManager::UnloadEffect(CBaseEffect* effect) {
	EffectList::iterator it;
	if (!EffectExists(effect->m_key, &it))
		return;

	effect->OnRelease();
	delete effect;

	m_effects.erase(it);
}

void CEffectManager::Begin() {
	TEMP_CONTEXT(pContext);

	ID3D11SamplerState* samplers[NUM_SAMPLERS] = {
		m_pSamplerClamp, m_pSamplerWrap, m_pSamplerShadow
	};

	pContext->PSSetSamplers(SAMPLER_SLOT_BEGIN, NUM_SAMPLERS, samplers);
	if (!m_bIsShading)
		pContext->PSSetShader(nullptr, nullptr, 0);

	m_currentEffectKey = 0;
}

void CEffectManager::ApplyEffect(CBaseEffect* effect) {
	TEMP_CONTEXT(pContext);

	if (!EffectExists(effect->m_key))
		return;

	if (effect->m_key == m_currentEffectKey)
		return;

	m_currentEffectKey = effect->m_key;
	effect->OnApply(pContext, m_bIsShading);
}

bool CEffectManager::EffectExists(EffectKey key, EffectList::iterator* outIt) {
	EffectList::iterator it = m_effects.find(key);
	if (outIt)
		*outIt = it;
	return (it != m_effects.end());
}



void CInstanceEffect::CreateInputLayout(GraphicsDevice* pDevice, ShaderSource vs) {
	int numElements = 1;
	D3D11_INPUT_ELEMENT_DESC inputElements[5];

	inputElements[0].SemanticName = "POSITION";
	inputElements[0].SemanticIndex = 0;
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[0].InputSlot = 0;
	inputElements[0].AlignedByteOffset = 0;
	inputElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElements[0].InstanceDataStepRate = 0;

	if (m_texCoordType != TexCoordNone) {
		numElements++;
		DXGI_FORMAT format = (m_texCoordType == TexCoord2D ? DXGI_FORMAT_R32G32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT);
		inputElements[numElements - 1].SemanticName = "TEXCOORD";
		inputElements[numElements - 1].SemanticIndex = 0;
		inputElements[numElements - 1].Format = format;
		inputElements[numElements - 1].InputSlot = 0;
		inputElements[numElements - 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[numElements - 1].InstanceDataStepRate = 0;
	}

	if (m_bUseNormal) {
		numElements++;
		inputElements[numElements - 1].SemanticName = "NORMAL";
		inputElements[numElements - 1].SemanticIndex = 0;
		inputElements[numElements - 1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[numElements - 1].InputSlot = 0;
		inputElements[numElements - 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[numElements - 1].InstanceDataStepRate = 0;
	}

	if (m_bUseTangent) {
		numElements++;
		inputElements[numElements - 1].SemanticName = "TANGENT";
		inputElements[numElements - 1].SemanticIndex = 0;
		inputElements[numElements - 1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElements[numElements - 1].InputSlot = 0;
		inputElements[numElements - 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[numElements - 1].InstanceDataStepRate = 0;
	}

	numElements++;
	inputElements[numElements - 1].SemanticName = "WORLDPOSITION";
	inputElements[numElements - 1].SemanticIndex = 0;
	inputElements[numElements - 1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElements[numElements - 1].InputSlot = 1;
	inputElements[numElements - 1].AlignedByteOffset = 0;
	inputElements[numElements - 1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	inputElements[numElements - 1].InstanceDataStepRate = 1;

	pDevice->CreateInputLayout(inputElements, numElements, vs.pByteCode, vs.size, &m_pInputLayout);
}