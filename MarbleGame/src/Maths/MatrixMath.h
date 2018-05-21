//
//  MatrixMath.h
//  Terrain
//
//  Created by Tarik Karaca on 10.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __Terrain__MatrixMath__
#define __Terrain__MatrixMath__

#include <memory>

union Vector3;
union Vector4;
union Quaternion;

union Matrix4 {
    float m[16];
    
	void SetFromArray(const float mat[16]) {
		memcpy(m, mat, 16);
	}

    Vector3 operator*(const Vector3& v);
    Vector4 operator*(const Vector4& v);
    Matrix4 operator*(const Matrix4& m);
    
    void operator*=(const Matrix4& m);
    
    Matrix4 Transpose() const;

	static Matrix4 Identity() {
		Matrix4 temp = { 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };
		return temp;
	}

	static Matrix4 LookAt();
	static Matrix4 Ortho(float, float, float, float, float, float);
	static Matrix4 Perspective(float, float, float, float);
};

#define MATRIX4_IDENTITY Matrix4::Identity()

Matrix4 Matrix(float* m);
Matrix4 Matrix(const Quaternion&, const Vector3&);
Matrix4 Matrix(const Quaternion& rot, const Vector3& translation, const Vector3& scale);
Matrix4 MatrixFromQuaternion(float x, float y, float z, float w);
Matrix4 MatrixFromQuaternion(Quaternion q);

Matrix4 GetMatrix3(Matrix4 matrix);

Vector3 MatrixMultiply(Matrix4 matrix, Vector3 vector);
Vector4 MatrixMultiply(Matrix4 matrix, Vector4 vector);
Matrix4 MatrixMultiply(Matrix4 matrixLeft, Matrix4 matrixRight);
Matrix4 MatrixInvert(Matrix4 matrix);
Matrix4 MatrixTranspose(Matrix4 matrix);

Matrix4 MatrixMakeOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
Matrix4 MatrixMakePerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
Matrix4 MatrixMakeLookAt(Vector3 eye, Vector3 center, Vector3 up);

Matrix4 MatrixIdentity();
Matrix4 MatrixBias();
Matrix4 MatrixTranslate(Vector3 translation);
Matrix4 MatrixScale(float x, float y, float z);
Matrix4 MatrixScale(Vector3 scale);
Matrix4 MatrixMakeRotationY(float rotation);
Matrix4 MatrixMakeRotationX(float rotation);
Matrix4 MatrixMakeRotationZ(float rotation);

inline void Matrix4::operator*=(const Matrix4 &m) {
    *this = MatrixMultiply(*this, m);
}

inline Matrix4 Matrix4::Transpose() const {
    return MatrixTranspose(*this);
}

#endif /* defined(__Terrain__MatrixMath__) */
