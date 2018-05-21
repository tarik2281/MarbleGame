//
//  Font.h
//  MarbleGame
//
//  Created by Tarik Karaca on 08.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__Font__
#define __MarbleGame__Font__

#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Resources/Resource.h"
#include "Maths/MatrixMath.h"
#include "Maths/VectorMath.h"
#include "CGraphics.h"


#ifdef _DEBUG
#define FREETYPE_LIBRARY "freetype253_D.lib"
#else
#define FREETYPE_LIBRARY "freetype253.lib"
#endif

#pragma comment(lib, FREETYPE_LIBRARY)

class CBaseEffect;
class CEffectManager;

struct Glyph {
	FT_Pos m_width;
	FT_Pos m_height;
	FT_Pos m_bearingY;
};

struct GlyphVertex {
	Vector3 position;
	Vector2 texCoord;
};

__declspec(align(16))
struct FontData {
	float fScreenWidth;
	float fScreenHeight;
	float fDestAngle;
};

typedef std::vector<GlyphVertex> VertexList;

#define MAX_ROW_SIZE 16

class FontLabel {
	friend class Font;
	ID3D11Buffer* vertexBuffer;
	Font* parent;
	unsigned int vertexCount;
	unsigned int maxSize;

public:
	void Render();
	void Release();

	void SetText(const std::wstring&, Vector2);
};

class Font : public IResource {
	typedef std::vector<FontLabel*> LabelList;
	friend class FontLabel;

	CEffectManager* m_pManager;
	CBaseEffect* m_pEffect;
	Glyph m_glyphs[256];

	unsigned int m_pixelSize;
	unsigned int m_imgWidth;
	unsigned int m_imgHeight;

	LabelList labels;
public:
	void LoadResource(const char* path);
	void Release();

	Vector2 GetSize(const std::wstring&);
	void MakeText(VertexList&, const std::wstring&, Vector2);
	FontLabel* MakeNewLabel(const std::wstring&, Vector2);

	ID3D11ShaderResourceView* m_texture;
	int m_charWidth;
	int m_charHeight;

	Font(CEffectManager* pManager) {
		m_pManager = pManager;
	}
};

extern Font* g_pFont;

#endif /* defined(__MarbleGame__Font__) */
