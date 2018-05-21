//
//  Skybox.h
//  MarbleGame
//
//  Created by Tarik Karaca on 28.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#define SKYBOX_SIZE 200.0f

#include "SceneNode.h"

class Texture2DArray;
class ID3D11Buffer;
class ID3D11DepthStencilState;

class SkyboxNode : public SceneNode {
public:
    void init();

protected:
	void OnRender();
	void OnRelease();
	void OnAdded();
	void OnRemoved();

	RenderPass GetRenderPass() const { return RenderPassPre; }
	bool HasShadows() const { return false; }

private:
    enum Textures {
        TexturesLeft,
        TexturesFront,
        TexturesRight,
        TexturesBack,
        TexturesTop,
        TexturesBottom,
        TexturesLength
    };
    
    struct Vertex {
        Vector3 position;
        Vector3 texCoord;
    };
    
	Texture2DArray* m_pTextures;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11DepthStencilState* m_pDSState;

	CBaseEffect* m_pEffect;
};

#endif // _SKYBOX_H_
