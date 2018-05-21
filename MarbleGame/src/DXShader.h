
#ifndef _DXSHADER_H_
#define _DXSHADER_H_

#include <map>
#include <string>

#ifdef _DEBUG
#include <d3dcompiler.h>
#endif

#include "CGraphics.h"
#include "Resources/Resource.h"
#include "Maths/MatrixMath.h"
#include "Maths/VectorMath.h"

#define SAMPLER_SLOT_BEGIN 0
#define SAMPLER_SLOT_CLAMP 0
#define SAMPLER_SLOT_WRAP 1
#define SAMPLER_SLOT_SHADOW 2
#define NUM_SAMPLERS 3

typedef Hash EffectKey;

enum TexCoordType {
	TexCoordNone,
	TexCoord2D,
	TexCoord3D
};

struct ShaderSource {
	char* pByteCode;
	UINT size;
};

class CBaseEffect {
public:
	virtual void Initialize(GraphicsDevice*, ShaderSource vs, ShaderSource ps);
	//virtual void Release();

	CBaseEffect() {
		m_pInputLayout = nullptr;
		m_pVertexShader = nullptr;
		m_pPixelShader = nullptr;
		m_texCoordType = TexCoordNone;
		m_bUseNormal = false;
		m_bUseTangent = false;
	}
	~CBaseEffect() { }
protected:
	friend class CEffectManager;

	virtual void CreateInputLayout(GraphicsDevice*, ShaderSource);
	virtual void OnApply(GraphicsContext*, bool usePS);
	virtual void OnRelease();

	ID3D11InputLayout* m_pInputLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	TexCoordType m_texCoordType;
	bool m_bUseNormal;
	bool m_bUseTangent;
	EffectKey m_key;
};

class CEffectManager {
public:
	void Initialize();
	void Release();

	void SetIsShading(bool);

	template<class T>
	T* LoadEffect(const char* name, TexCoordType texCoordType = TexCoord2D, bool useNormal = true, bool useTangent = false);
	CBaseEffect* LoadEffect(const char* name, TexCoordType texCoordType = TexCoord2D, bool useNormal = true, bool useTangent = false);

	void UnloadEffect(CBaseEffect*);

	void Begin();
	void ApplyEffect(CBaseEffect*);

	static void SetShadersPath(const std::string& path) {
		s_shaderPath = path;
	}

	CEffectManager() {
		m_pSamplerShadow = nullptr;
		m_bIsShading = false;
	}
private:
	typedef std::pair<EffectKey, CBaseEffect*> EffectPair;
	typedef std::map<EffectKey, CBaseEffect*> EffectList;

	bool EffectExists(EffectKey key, EffectList::iterator* outIt = nullptr);

	ID3D11SamplerState* m_pSamplerClamp;
	ID3D11SamplerState* m_pSamplerWrap;
	ID3D11SamplerState* m_pSamplerShadow;

	bool m_bIsShading;
	EffectList m_effects;
	EffectKey m_currentEffectKey;

	static std::string s_shaderPath;
};

inline void CEffectManager::SetIsShading(bool isShading) {
	m_bIsShading = isShading;
}

template<class T>
T* CEffectManager::LoadEffect(const char* name, TexCoordType texCoordType, bool useNormal, bool useTangent) {
#define TYPE T
	
	EffectKey key = StringHash(name);
	EffectList::iterator it;
	if (EffectExists(key, &it))
		return static_cast<TYPE*>(it->second);

	TYPE* effect = DBG_NEW TYPE();

	effect->m_texCoordType = texCoordType;
	effect->m_bUseNormal = useNormal;
	effect->m_bUseTangent = useTangent;
	effect->m_key = key;

	ShaderSource vsSource;
	ShaderSource psSource;
	memset(&vsSource, 0, sizeof(ShaderSource));
	memset(&psSource, 0, sizeof(ShaderSource));

	std::string vsFilePath = (s_shaderPath + name) + "_V.cso";
	std::string psFilePath = (s_shaderPath + name) + "_P.cso";

	FILE* vsFile = fopen(vsFilePath.c_str(), "rb");
	fseek(vsFile, 0, SEEK_END);
	vsSource.size = ftell(vsFile);
	fseek(vsFile, 0, SEEK_SET);

	vsSource.pByteCode = DBG_NEW char[vsSource.size];
	fread(vsSource.pByteCode, 1, vsSource.size, vsFile);
	fclose(vsFile);


	FILE* psFile = fopen(psFilePath.c_str(), "rb");
	fseek(psFile, 0, SEEK_END);
	psSource.size = ftell(psFile);
	fseek(psFile, 0, SEEK_SET);

	psSource.pByteCode = DBG_NEW char[psSource.size];
	fread(psSource.pByteCode, 1, psSource.size, psFile);
	fclose(psFile);

	effect->Initialize(g_pGraphics->GetDevice(), vsSource, psSource);

	if (vsSource.pByteCode)
		delete[] vsSource.pByteCode;
	if (psSource.pByteCode)
		delete[] psSource.pByteCode;

	m_effects.insert(EffectPair(key, effect));

	return effect;
#undef TYPE
}


inline CBaseEffect* CEffectManager::LoadEffect(const char* name, TexCoordType texCoordType, bool useNormal, bool useTangent) {
	return LoadEffect<CBaseEffect>(name, texCoordType, useNormal, useTangent);
}



class CInstanceEffect : public CBaseEffect {
public:
	CInstanceEffect() : CBaseEffect() {

	}
	~CInstanceEffect() { }
protected:
	void CreateInputLayout(GraphicsDevice*, ShaderSource) override;

};

#endif
