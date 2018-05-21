#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "StringHash.h"
#include "Maths/VectorMath.h"

class Texture2D;
typedef Vector4 Color;

struct Material {
	Hash identifier;
	float opacity;
	Texture2D* diffuseMap;
	Texture2D* normalMap;
};

class CLight {
	float m_attenuation[3];
};

class CMaterial {
	Color m_ambientColor;
	Color m_diffuseColor;
	Color m_specularColor;
};

#endif;