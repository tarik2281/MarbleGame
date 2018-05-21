//
//  Terrain.h
//  MarbleGame
//
//  Created by Tarik Karaca on 01.03.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__Terrain__
#define __MarbleGame__Terrain__

#include "SceneNode.h"
#include "Maths/VectorMath.h"
#include "../CGraphics.h"

class TerrainNode;
class CBaseEffect;

struct WaterVertex {
    Vector3 position;
    Vector2 texCoord;
};

class WaterNode : public SceneNode {
private:
	ID3D11RenderTargetView* m_pRefractionView;
	ID3D11ShaderResourceView* m_pRefractionResource;
	ID3D11RenderTargetView* m_pReflectionView;
	ID3D11ShaderResourceView* m_pReflectionResource;
	ID3D11DepthStencilView* m_pDepthBuffer;

	ID3D11RenderTargetView* m_pWavesView;
	ID3D11ShaderResourceView* m_pWavesResource;

	ID3D11RasterizerState* m_pRSFrontCulling;
    
    Texture2D* wavesTexture;
    
    friend class TerrainNode;
    friend class WaterController;
    TerrainNode* terrain;
	Vector4 ClipPlane;
	ID3D11Buffer* m_pVertexBuffer;
	CBaseEffect* m_pShader;
	
    Vector2 wavesPosition;
public:
    
    void init();
    
    void setTerrain(TerrainNode*);
protected:
	void OnRender();
	void OnRelease();
	void OnAdded();

	bool HasShadows() const { return false; }
};

class TerrainNode : public SceneNode {
private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
    unsigned int indexBufferSize;
    Texture2D* texture;
	CBaseEffect* m_pShader;
public:
    void init();
protected:
	void OnRender();
	void OnRelease();
	void OnAdded();

	bool HasShadows() const { return false; }
};

#endif /* defined(__MarbleGame__Terrain__) */
