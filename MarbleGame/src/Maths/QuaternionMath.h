#ifndef _QUATERNION_MATH_H_
#define _QUATERNION_MATH_H_

#define _USE_MATH_DEFINES
#include <math.h>

#include <LinearMath/btQuaternion.h>

#include "VectorMath.h"
#include "MatrixMath.h"

union Quaternion {
	struct {
		float x, y, z, w;
	};
	struct {
		Vector3 xyz;
		float w;
	};
	float q[4];


	Quaternion() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	Quaternion(float pX, float pY, float pZ, float pW) {
		x = pX;
		y = pY;
		z = pZ;
		w = pW;
	}

	Quaternion(const Vector3& axis, float angle) {
		float rad = angle / 2.0f * M_PI / 180.0f;
		float sin = sinf(rad);
		x = axis.x * sin;
		y = axis.y * sin;
		z = axis.z * sin;
		w = cosf(rad);
	}

	Quaternion(const btQuaternion& q) {
		x = q.x();
		y = q.y();
		z = q.z();
		w = q.w();
	}


	float Dot(const Quaternion& pQ) const {
		return x * pQ.x + y * pQ.y + z * pQ.z + w * pQ.w;
	}

	float LengthSQ() const {
		return Dot(*this);
	}

	float Length() const {
		return sqrtf(LengthSQ());
	}

	Quaternion Normalize() const {
		float l = Length();
		return Quaternion(x / l, y / l, z / l, w / l);
	}

	Quaternion Conjugate() const {
		return Quaternion(-x, -y, -z, w);
	}

	Quaternion Invert() const {
		return Conjugate() / LengthSQ();
	}

	Matrix4 ToMatrix() const {
		float _2x = x + x;
		float _2y = y + y;
		float _2z = z + z;

		Matrix4 res = { 1.0f - _2y * y - _2z * z, _2x * y + _2z * w, _2x * z - _2y * w, 0,
						_2x * y - _2z * w, 1.0f - _2x * x - _2z * z, _2y * z + _2x * w, 0,
						_2x * z + _2y * w, _2y * z - _2x * w, 1.0f - _2x * x - _2y * y, 0,
						0, 0, 0, 1 };

		return res;
	}


	Quaternion operator+(const Quaternion& pQ) const {
		return Quaternion(x + pQ.x, y + pQ.y, z + pQ.z, w + pQ.w);
	}

	Quaternion& operator+=(const Quaternion& pQ) {
		*this = *this + pQ;
		return *this;
	}


	Quaternion operator*(const Quaternion& pQ) const {
		Quaternion temp;

		temp.w = (w * pQ.w - x * pQ.x - y * pQ.y - z * pQ.z);
		temp.x = (w * pQ.x + x * pQ.w + y * pQ.z - z * pQ.y);
		temp.y = (w * pQ.y - x * pQ.z + y * pQ.w + z * pQ.x);
		temp.z = (w * pQ.z + x * pQ.y - y * pQ.x + z * pQ.w);

		return temp;
	}

	Vector3 operator*(const Vector3& pV) const {
		Vector3 t = xyz.Cross(pV) * 2.0f;
		return (pV + t * w + xyz.Cross(t));
	}

	Quaternion operator*(float f) const {
		return Quaternion(x * f, y * f, z * f, w * f);
	}

	Quaternion& operator*=(const Quaternion& pQ) {
		*this = *this * pQ;
		return *this;
	}

	Quaternion& operator*=(float f) {
		*this = *this * f;
		return *this;
	}

	Quaternion operator/(float f) const {
		return Quaternion(x / f, y / f, z / f, w / f);
	}

	Quaternion& operator/=(float f) {
		*this = *this / f;
		return *this;
	}


	operator btQuaternion() const {
		return btQuaternion(x, y, z, w);
	}
};

#define QUATERNION_IDENTITY Quaternion()

#endif /* _QUATERNION_MATH_H_ */