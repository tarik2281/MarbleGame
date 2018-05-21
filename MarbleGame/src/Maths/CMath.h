#ifndef _CMATH_H_
#define _CMATH_H_

#include <math.h>

class CMath {
public:
	static const float PI;

	static float Abs(float f) { return fabsf(f); }

	static float Sin(float f) { return sinf(f); }
	static float Cos(float f) { return cosf(f); }
	static float Tan(float f) { return tanf(f); }

	static float Asin(float f) { return asinf(f); }
	static float Acos(float f) { return acosf(f); }
	static float Atan(float f) { return atanf(f); }
	static float Atan2(float y, float x) { return atan2f(y, x); }

	static float Sqrt(float f) { return sqrtf(f); }

	static float ToDegrees(float f) { return f * 180.0f / PI; }
	static float ToRadians(float f) { return f * PI / 180.0f; }
};

#endif // _CMATH_H_