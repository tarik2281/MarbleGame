//
//  Resource.h
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__Resource__
#define __MarbleGame__Resource__

#include "Maths/VectorMath.h"
#include "../CGraphics.h"
#include "StringHash.h"

class ResourceManager;

class IResource {
public:
    virtual void LoadResource(const char* path) = 0;
    virtual void Release() = 0;
    virtual void Copy(IResource* destination) { };
protected:
	friend class ResourceManager;

	IResource() {
		m_pResourceManager = nullptr;
		m_resourceKey = 0;
	}

	ResourceManager* m_pResourceManager;

	Hash m_resourceKey;
};


class Texture2D : public IResource {
public:
    void Release();
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    void LoadResource(const char* path);
    void Copy(IResource* destination);

	ID3D11ShaderResourceView* m_pView;
	ID3D11Texture2D* m_pTexture;
private:
    int m_width;
    int m_height;
};

class Texture2DArray : public IResource {
public:
	void Release();
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }

	void LoadResource(const char* path);
	void Copy(IResource* destination);

	ID3D11ShaderResourceView* m_pView;
	ID3D11Texture2D* m_pTexture;

	Texture2DArray() : IResource() {
		m_width = 0;
		m_height = 0;
		m_pTextureNames = nullptr;
		m_numTextures = 0;
	}
private:
	friend class ResourceManager;

	void SetTextureNames(const char** paths, int numTextures);

	int m_width;
	int m_height;
	const char** m_pTextureNames;
	int m_numTextures;
};

inline void Texture2D::Copy(IResource *destination) {
    Texture2D* res = static_cast<Texture2D*>(destination);
    res->m_width = m_width;
    res->m_height = m_height;
}

inline void Texture2DArray::Copy(IResource* destination) {

}

#endif /* defined(__MarbleGame__Resource__) */
