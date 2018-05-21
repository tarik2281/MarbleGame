//
//  VectorMath.h
//  Terrain
//
//  Created by Tarik Karaca on 08.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __Terrain__VectorMath__
#define __Terrain__VectorMath__

//#ifdef USE_BULLET_PHYSICS
#include <math.h>

#include <LinearMath/btvector3.h>
//#endif

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)

union Vector2 {
    struct {
        float x, y;
    };
	struct {
		float s, t;
	};
    float v[2];
    
    Vector2() {
		this->x = 0.0f;
		this->y = 0.0f;
	}

	Vector2(float x) {
		this->x = x;
		this->y = x;
	}

    Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}
    
    bool operator==(const Vector2& v) const {
        return ((x == v.x) && (y == v.y));
    }
    
    bool operator<(const Vector2& v) const {
        return x < v.x || (x == v.x && y < v.y);
    }


	Vector2 operator-() const {
		return Vector2(-x, -y);
	}


	Vector2 operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 operator*(float f) const {
		return Vector2(x * f, y * f);
	}

	Vector2 operator/(float f) const {
		return Vector2(x / f, y / f);
	}

    
	void operator+=(const Vector2& v) {
		*this = *this + v;
	}

	void operator-=(const Vector2& v) {
		*this = *this - v;
	}

	void operator*=(float f) {
		*this = *this * f;
	}

	void operator/=(float f) {
		*this = *this / f;
	}

    
	float Dot(const Vector2& v) const {
		return x * v.x + y * v.y;
	}

	float Length() const {
		return sqrtf(x * x + y * y);
	}

	float LengthSQ() const {
		return x * x + y * y;
	}

	Vector2 Normalize() const {
		float l = Length();
		return Vector2(x / l, y / l);
	}

	Vector2 Min(const Vector2& v) const {
		Vector2 temp;
		temp.x = min(x, v.x);
		temp.y = min(y, v.y);
		return temp;
	}

	Vector2 Max(const Vector2& v) const {
		Vector2 temp;
		temp.x = max(x, v.x);
		temp.y = max(y, v.y);
		return temp;
	}
};

#define VECTOR2_ZERO Vector2()
#define VECTOR2_UP Vector2(0.0f, -1.0f)
#define VECTOR2_DOWN Vector2(0.0f, 1.0f)
#define VECTOR2_LEFT Vector2(-1.0f, 0.0f)
#define VECTOR2_RIGHT Vector2(1.0f, 0.0f)

union Vector3 {
    struct {
        float x, y, z;
    };
	struct {
		Vector2 xy;
		float z;
	};
	struct {
		float r, g, b;
	};
    float v[3];
    
    Vector3() {
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	Vector3(float x) {
		this->x = x;
		this->y = x;
		this->z = x;
	}

	Vector3(const Vector2& pV, float pZ) {
		x = pV.x;
		y = pV.y;
		z = pZ;
	}

    Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
    
    bool operator==(const Vector3& v) const {
        return ((x == v.x) && (y == v.y) && (z == v.z));
    }

	bool operator!=(const Vector3& v) const {
		return !(*this == v);
	}

	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}

    
	Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(const Vector3& v) const {
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 operator*(float f) const {
		return Vector3(x * f, y * f, z * f);
	}

	Vector3 operator/(float f) const {
		return Vector3(x / f, y / f, z / f);
	}
    
	void operator+=(const Vector3& v) {
		*this = *this + v;
	}

	void operator-=(const Vector3& v) {
		*this = *this - v;
	}

	void operator*=(float f) {
		*this = *this * f;
	}

	void operator/=(float f) {
		*this = *this / f;
	}
    
	float Dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float LengthSQ() const {
		return Dot(*this);
	}
	
	float Length() const {
		return sqrtf(LengthSQ());
	}

	Vector3 Normalize() const {
		float l = Length();
		return *this / l;
	}

	Vector3 Cross(const Vector3& v) const {
		Vector3 temp;
		temp.x = (y * v.z) - (z * v.y);
		temp.y = (z * v.x) - (x * v.z);
		temp.z = (x * v.y) - (y * v.x);
		return temp;
	}

	Vector3 SwapXZ() const {
		return Vector3(z, y, x);
	}

	Vector3 Min(const Vector3& v) const {
		Vector3 temp;
		temp.x = min(x, v.x);
		temp.y = min(y, v.y);
		temp.z = min(z, v.z);
		return temp;
	}

	Vector3 Max(const Vector3& v) const {
		Vector3 temp;
		temp.x = max(x, v.x);
		temp.y = max(y, v.y);
		temp.z = max(z, v.z);
		return temp;
	}
    
	Vector3(const btVector3& v) {
		x = v.x();
		y = v.y();
		z = v.z();
	}

	operator btVector3() const {
		return btVector3(x, y, z);
	}

	void operator=(const btVector3& vec) {
		this->x = vec.x();
		this->y = vec.y();
		this->z = vec.z();
	}

	Vector3 operator+(const btVector3& v) const {
		return *this + Vector3(v);
	}

	Vector3 operator-(const btVector3& v) const {
		return *this - Vector3(v);
	}

	void operator+=(const btVector3& v) {
		*this = *this + v;
	}

	void operator-=(const btVector3& v) {
		*this = *this - v;
	}
};

#define VECTOR3_ZERO Vector3(0.0f, 0.0f, 0.0f)
#define VECTOR3_UP Vector3(0.0f, 1.0f, 0.0f)
#define VECTOR3_RIGHT Vector3(1.0f, 0.0f, 0.0f)
#define VECTOR3_LEFT Vector3(-1.0f, 0.0f, 0.0f)
#define VECTOR3_DOWN Vector3(0.0f, -1.0f, 0.0f)
#define VECTOR3_FORWARD Vector3(0.0f, 0.0f, -1.0f)
#define VECTOR3_BACKWARD Vector3(0.0f, 0.0f, 1.0f)

union Vector4 {
    struct {
        float x, y, z, w;
	};
	struct {
		float r, g, b, a;
	};
    float v[4];
    
    Vector4() { }
	Vector4(float x) : x(x), y(x), z(x), w(x) { }
	Vector4(const Vector3& v, float pW) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = pW;
	}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
    
    

	float Dot(const Vector4& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	float LengthSQ() const {
		return Dot(*this);
	}

	float Length() const {
		return sqrtf(LengthSQ());
	}

	Vector4 Normalize() const {
		float l = Length();
		return *this / l;
	}

	bool operator==(const Vector4& v) const {
		return ((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w));
	}

	bool operator!=(const Vector4& v) const {
		return !(*this == v);
	}

	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}


	Vector4 operator+(const Vector4& v) const {
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4 operator-(const Vector4& v) const {
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4 operator*(float f) const {
		return Vector4(x * f, y * f, z * f, w * f);
	}

	Vector4 operator*(const Vector4& v) const {
		return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	Vector4 operator/(float f) const {
		return Vector4(x / f, y / f, z / f, w / f);
	}

	Vector4 operator/(const Vector4& v) const {
		return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	void operator+=(const Vector4& v) {
		*this = *this + v;
	}

	void operator-=(const Vector4& v) {
		*this = *this - v;
	}

	void operator*=(float f) {
		*this = *this * f;
	}

	void operator*=(const Vector4& v) {
		*this = *this * v;
	}

	void operator/=(float f) {
		*this = *this / f;
	}

	void operator/=(const Vector4& v) {
		*this = *this / v;
	}

	Vector3 xyz() const {
		return Vector3(x, y, z);
	}

	Vector4 Min(const Vector4& v) const {
		Vector4 temp;
		temp.x = min(x, v.x);
		temp.y = min(y, v.y);
		temp.z = min(z, v.z);
		temp.w = min(w, v.w);
		return temp;
	}

	Vector4 Max(const Vector4& v) const {
		Vector4 temp;
		temp.x = max(x, v.x);
		temp.y = max(y, v.y);
		temp.z = max(z, v.z);
		temp.w = max(w, v.w);
		return temp;
	}
};

inline Vector4 Vector4Min(const Vector4& v1, const Vector4& v2) {
	Vector4 v;
	v.x = min(v1.x, v2.x);
	v.y = min(v1.y, v2.y);
	v.z = min(v1.z, v2.z);
	v.w = min(v1.w, v2.w);
	return v;
}

inline Vector4 Vector4Max(const Vector4& v1, const Vector4& v2) {
	Vector4 v;
	v.x = max(v1.x, v2.x);
	v.y = max(v1.y, v2.y);
	v.z = max(v1.z, v2.z);
	v.w = max(v1.w, v2.w);
	return v;
}

inline float roundDouble(float f) {
	return floorf(f + 0.5f);
}

#define VectorZero Vector3(0.0f, 0.0f, 0.0f)
#define VectorUp Vector3(0.0f, 1.0f, 0.0f)
#define VectorLeft Vector3(1.0f, 0.0f, 0.0f)
#define VectorRight Vector3(-1.0f, 0.0f, 0.0f)
#define VectorDown Vector3(0.0f, -1.0f, 0.0f)
#define VectorForward Vector3(0.0f, 0.0f, -1.0f)
#define VectorBackward Vector3(0.0f, 0.0f, 1.0f)

#endif /* defined(__Terrain__VectorMath__) */
