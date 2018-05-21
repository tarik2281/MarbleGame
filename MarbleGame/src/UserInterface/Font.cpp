//
//  Font.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 08.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#include <stdint.h>
#include <math.h>

#include <png.h>

#include "Font.h"
#include "Resources/ResourceManager.h"
#include "../DXShader.h"
#include "Util.h"
#include "../main.h"

#include FT_GLYPH_H

Font* g_pFont = 0;

#define SPACE_WIDTH 12

void FontLabel::Release() {
	vertexBuffer->Release();
	delete this;
}

void FontLabel::Render() {
	GraphicsContext* pContext = g_pGraphics->GetContext();

	UINT stride = sizeof(GlyphVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pContext->PSSetShaderResources(0, 1, &parent->m_texture);
	
	parent->m_pManager->ApplyEffect(parent->m_pEffect);
	pContext->Draw(vertexCount, 0);
}

void FontLabel::SetText(const std::wstring& text, Vector2 position) {
	VertexList vertices;
	parent->MakeText(vertices, text, position);

	if (vertices.size() * sizeof(GlyphVertex) > maxSize) {
		vertexBuffer->Release();
		maxSize = vertices.size() * sizeof(GlyphVertex);

		D3D11_BUFFER_DESC buffDesc;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.ByteWidth = maxSize;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;

		g_pGraphics->GetDevice()->CreateBuffer(&buffDesc, NULL, &vertexBuffer);
	}

	GraphicsContext* pContext = g_pGraphics->GetContext();

	D3D11_MAPPED_SUBRESOURCE data;
	GlyphVertex* v;
	pContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	v = (GlyphVertex*)data.pData;
	memcpy(v, &vertices[0], maxSize);
	pContext->Unmap(vertexBuffer, 0);
	vertexCount = vertices.size();
}


void Font::LoadResource(const char *path) {
    m_pixelSize = 20;
    int rows = 256 / MAX_ROW_SIZE;
    m_imgWidth = MAX_ROW_SIZE * m_pixelSize;
    m_imgHeight = rows * m_pixelSize;
    
    FT_Library library;
    FT_Face face;
    uint8_t* imgData;
    
    if (FT_Init_FreeType(&library)) {
        fprintf(stderr, "Could not initialize FreeType\n");
        return;
    }
    
    if (FT_New_Face(library, "C:\\Windows\\Fonts\\impact.ttf", 0, &face)) {
        fprintf(stderr, "Could not open/read font file\n");
        FT_Done_FreeType(library);
        return;
    }
    
    //FT_Set_Pixel_Sizes(face, m_pixelSize + 1, m_pixelSize);
    FT_Set_Char_Size(face, 0, m_pixelSize*64, 72, 72);
    imgData = DBG_NEW uint8_t[2 * m_imgWidth * m_imgHeight];
    
    for (int c = 0x21; c < 256; c++) {
        FT_Glyph glyph;
        FT_UInt glyphIndex;
        int x = (c - 0x21) % MAX_ROW_SIZE;
        int y = (c - 0x21) / MAX_ROW_SIZE;
        x *= m_pixelSize;
        y *= m_pixelSize;
        
        glyphIndex = FT_Get_Char_Index(face, c);
        if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
            fprintf(stderr, "Could not load glyph for %c(0x%2x)\n", c, c);
            continue;
        }
        
        if (FT_Get_Glyph(face->glyph, &glyph)) {
            fprintf(stderr, "Could not get glyph for %c(0x%2x)\n", c, c);
            continue;
        }
        
        if (FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1)) {
            fprintf(stderr, "Failed on rendering glyph to bitmap\n");
            continue;
        }
        
        FT_Bitmap& bitmap = ((FT_BitmapGlyph)glyph)->bitmap;
        m_glyphs[c].m_width = face->glyph->metrics.width / 58;
        m_glyphs[c].m_height = face->glyph->metrics.height / 58;
        m_glyphs[c].m_bearingY = face->glyph->metrics.horiBearingY / 58;
        if (c == 0x28 || c == 0x29 || c == 0x5b || c == 0x5d || c == 0x7d || c == 0x7b)
            m_glyphs[c].m_bearingY = face->glyph->metrics.height / 58 - 3;
        
        for (int i = 0; i < m_pixelSize; i++) {
            for (int j = 0; j < bitmap.rows; j++) {
                int index = 2 * (x + i + (j + y) * m_imgWidth);
                imgData[index] = imgData[index + 1] = (i >= bitmap.width || j >= bitmap.rows)
                    ? 0 : bitmap.buffer[i + j * bitmap.width];
            }
        }
        
        FT_Done_Glyph(glyph);
    }
    

	TEMP_DEVICE(pDevice);

	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R8G8_UNORM;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = m_imgWidth;
	desc.Height = m_imgHeight;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = imgData;
	data.SysMemPitch = m_imgWidth * 2;
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* texture;

	HRESULT res = pDevice->CreateTexture2D(&desc, &data, &texture);

	if (FAILED(res))
		MessageBoxA(NULL, "Could not create font texture", "Error", MB_OK);

	pDevice->CreateShaderResourceView(texture, NULL, &m_texture);

	texture->Release();
    
    delete [] imgData;
    
    FT_Done_Face(face);
    FT_Done_FreeType(library);

	m_pEffect = m_pManager->LoadEffect("FontShader", TexCoord2D, false, false);

	//m_shader = g_pResourceManager->LoadContent<Shader>("..\\FontShader.cso");
	//m_shader->Initialize("FontTechnique", "FontPass", VertexInputPosTex);

	Matrix4 ortho = MatrixMakeOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
	//m_shader->SetVariable("projectionMatrix", &ortho);
	//m_shader->SetVariable("fontTexture", m_texture);
}

void Font::Release() {
	m_texture->Release();

	LabelList::iterator it;
	for (it = labels.begin(); it != labels.end(); ++it) {
		(*it)->Release();
	}
}

Vector2 Font::GetSize(const std::wstring &text) {
	Vector2 size(0.0f);
    
    for (int i = 0; i < text.length(); i++) {
        int charPos = text[i];
        if (charPos == ' ') {
            size.x += SPACE_WIDTH;
            continue;
        }
        size.x += (int)m_glyphs[charPos].m_width + 2;
		size.y = max(size.y, (int)m_glyphs[charPos].m_height);
    }
    
    return size;
}

void Font::MakeText(VertexList& vertices, const std::wstring& text, Vector2 position) {
	vertices.reserve(text.length() * 6);

	int x = 0;
	for (int i = 0; i < text.length(); i++) {
		unsigned char charPos = text[i];
		if (charPos == ' ') {
			x += SPACE_WIDTH;
			continue;
		}

		int charWidth = (int)m_glyphs[charPos].m_width;
		int charHeight = (int)m_glyphs[charPos].m_height;
		int y = position.y - (m_glyphs[charPos].m_bearingY);

		float texX = (float)((charPos - 0x21) % MAX_ROW_SIZE);
		float texY = floorf(float(charPos - 0x21) / float(MAX_ROW_SIZE));

		texX *= m_pixelSize;
		texY *= m_pixelSize;

		charHeight -= 1;
		GlyphVertex v;
		v.position = Vector3(position.x + x, y, 0.0f);
		v.texCoord = Vector2(texX / float(m_imgWidth), texY / float(m_imgHeight));
		vertices.push_back(v);
		v.position = Vector3(position.x + x, y + charHeight, 0.0f);
		v.texCoord = Vector2(texX / float(m_imgWidth), (texY + charHeight) / float(m_imgHeight));
		vertices.push_back(v);
		v.position = Vector3(position.x + x + charWidth, y, 0.0f);
		v.texCoord = Vector2((texX + charWidth) / float(m_imgWidth), texY / float(m_imgHeight));
		vertices.push_back(v);
		vertices.push_back(v);
		v.position = Vector3(position.x + x, y + charHeight, 0.0f);
		v.texCoord = Vector2(texX / float(m_imgWidth), (texY + charHeight) / float(m_imgHeight));
		vertices.push_back(v);
		v.position = Vector3(position.x + x + charWidth, y + charHeight, 0.0f);
		v.texCoord = Vector2((texX + charWidth) / float(m_imgWidth), (texY + charHeight) / float(m_imgHeight));
		vertices.push_back(v);


		x += charWidth;
		x += 2;
	}
}

FontLabel* Font::MakeNewLabel(const std::wstring& text, Vector2 position) {
	FontLabel* label = new FontLabel();

	VertexList vertices;
	MakeText(vertices, text, position);

	D3D11_BUFFER_DESC buffDesc;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = vertices.size() * sizeof(GlyphVertex);
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertices[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	g_pGraphics->GetDevice()->CreateBuffer(&buffDesc, &data, &label->vertexBuffer);

	label->parent = this;
	label->vertexCount = vertices.size();
	label->maxSize = vertices.size() * sizeof(GlyphVertex);

	labels.push_back(label);
	return label;
}