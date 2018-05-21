//
//  MatrixMath.cpp
//  Terrain
//
//  Created by Tarik Karaca on 10.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "MatrixMath.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "VectorMath.h"
#include "QuaternionMath.h"

Vector3 Matrix4::operator*(const Vector3 &v) {
    return MatrixMultiply(*this, v);
}

Vector4 Matrix4::operator*(const Vector4 &v) {
    return MatrixMultiply(*this, v);
}

Matrix4 Matrix4::operator*(const Matrix4 &m) {
    return MatrixMultiply(*this, m);
}

Matrix4 Matrix(float* mat) {
    Matrix4 res;
	memcpy(res.m, mat, 16 * sizeof(float));
    return res;
}

Matrix4 Matrix(const Quaternion& orientation, const Vector3& position) {
    Matrix4 res = MatrixFromQuaternion(orientation);
    res.m[12] = position.x;
    res.m[13] = position.y;
    res.m[14] = position.z;
    return res;
}

Matrix4 Matrix(const Quaternion& rot, const Vector3& translation, const Vector3& scale) {
	return MatrixIdentity(); // implementation missing
}

Matrix4 MatrixFromQuaternion(float x, float y, float z, float w) {
    return MatrixFromQuaternion(Quaternion(x, y, z, w));
}

Matrix4 MatrixFromQuaternion(Quaternion q) {
    float _2x = q.x + q.x;
    float _2y = q.y + q.y;
    float _2z = q.z + q.z;
	
	Matrix4 res = { 1.0f - _2y * q.y - _2z * q.z,  _2x * q.y + _2z * q.w,  _2x * q.z - _2y * q.w, 0,
        _2x * q.y - _2z * q.w,  1.0f - _2x * q.x - _2z * q.z,  _2y * q.z + _2x * q.w, 0,
        _2x * q.z + _2y * q.w,  _2y * q.z - _2x * q.w,  1.0f - _2x * q.x - _2y * q.y, 0,
        0, 0, 0, 1 };
    
    return res;
}

Matrix4 GetMatrix3(Matrix4 matrix) {
    Matrix4 res = { matrix.m[0], matrix.m[1], matrix.m[2], 0.0f,
            matrix.m[4], matrix.m[5], matrix.m[6], 0.0f,
            matrix.m[8], matrix.m[9], matrix.m[10], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
    return res;
}

Vector3 MatrixMultiply(Matrix4 matrix, Vector3 vector) {
    Vector3 res;
    res.x = matrix.m[0] * vector.x + matrix.m[4] * vector.y + matrix.m[8] * vector.z + matrix.m[12] * 1.0f;
    res.y = matrix.m[1] * vector.x + matrix.m[5] * vector.y + matrix.m[9] * vector.z + matrix.m[13] * 1.0f;
    res.z = matrix.m[2] * vector.x + matrix.m[6] * vector.y + matrix.m[10] * vector.z + matrix.m[14] * 1.0f;
    return res;
}

Vector4 MatrixMultiply(Matrix4 matrix, Vector4 vector) {
    Vector4 res;
    res.x = matrix.m[0] * vector.x + matrix.m[4] * vector.y + matrix.m[8] * vector.z + matrix.m[12] * vector.w;
    res.y = matrix.m[1] * vector.x + matrix.m[5] * vector.y + matrix.m[9] * vector.z + matrix.m[13] * vector.w;
    res.z = matrix.m[2] * vector.x + matrix.m[6] * vector.y + matrix.m[10] * vector.z + matrix.m[14] * vector.w;
    res.w = matrix.m[3] * vector.x + matrix.m[7] * vector.y + matrix.m[11] * vector.z + matrix.m[15] * vector.w;
    return res;
}

Matrix4 MatrixMultiply(Matrix4 matrixLeft, Matrix4 matrixRight) {
    Matrix4 m;
    m.m[0]  = matrixLeft.m[0] * matrixRight.m[0]  + matrixLeft.m[4] * matrixRight.m[1]  + matrixLeft.m[8] * matrixRight.m[2]   + matrixLeft.m[12] * matrixRight.m[3];
	m.m[4]  = matrixLeft.m[0] * matrixRight.m[4]  + matrixLeft.m[4] * matrixRight.m[5]  + matrixLeft.m[8] * matrixRight.m[6]   + matrixLeft.m[12] * matrixRight.m[7];
	m.m[8]  = matrixLeft.m[0] * matrixRight.m[8]  + matrixLeft.m[4] * matrixRight.m[9]  + matrixLeft.m[8] * matrixRight.m[10]  + matrixLeft.m[12] * matrixRight.m[11];
	m.m[12] = matrixLeft.m[0] * matrixRight.m[12] + matrixLeft.m[4] * matrixRight.m[13] + matrixLeft.m[8] * matrixRight.m[14]  + matrixLeft.m[12] * matrixRight.m[15];
    
	m.m[1]  = matrixLeft.m[1] * matrixRight.m[0]  + matrixLeft.m[5] * matrixRight.m[1]  + matrixLeft.m[9] * matrixRight.m[2]   + matrixLeft.m[13] * matrixRight.m[3];
	m.m[5]  = matrixLeft.m[1] * matrixRight.m[4]  + matrixLeft.m[5] * matrixRight.m[5]  + matrixLeft.m[9] * matrixRight.m[6]   + matrixLeft.m[13] * matrixRight.m[7];
	m.m[9]  = matrixLeft.m[1] * matrixRight.m[8]  + matrixLeft.m[5] * matrixRight.m[9]  + matrixLeft.m[9] * matrixRight.m[10]  + matrixLeft.m[13] * matrixRight.m[11];
	m.m[13] = matrixLeft.m[1] * matrixRight.m[12] + matrixLeft.m[5] * matrixRight.m[13] + matrixLeft.m[9] * matrixRight.m[14]  + matrixLeft.m[13] * matrixRight.m[15];
    
	m.m[2]  = matrixLeft.m[2] * matrixRight.m[0]  + matrixLeft.m[6] * matrixRight.m[1]  + matrixLeft.m[10] * matrixRight.m[2]  + matrixLeft.m[14] * matrixRight.m[3];
	m.m[6]  = matrixLeft.m[2] * matrixRight.m[4]  + matrixLeft.m[6] * matrixRight.m[5]  + matrixLeft.m[10] * matrixRight.m[6]  + matrixLeft.m[14] * matrixRight.m[7];
	m.m[10] = matrixLeft.m[2] * matrixRight.m[8]  + matrixLeft.m[6] * matrixRight.m[9]  + matrixLeft.m[10] * matrixRight.m[10] + matrixLeft.m[14] * matrixRight.m[11];
	m.m[14] = matrixLeft.m[2] * matrixRight.m[12] + matrixLeft.m[6] * matrixRight.m[13] + matrixLeft.m[10] * matrixRight.m[14] + matrixLeft.m[14] * matrixRight.m[15];
    
	m.m[3]  = matrixLeft.m[3] * matrixRight.m[0]  + matrixLeft.m[7] * matrixRight.m[1]  + matrixLeft.m[11] * matrixRight.m[2]  + matrixLeft.m[15] * matrixRight.m[3];
	m.m[7]  = matrixLeft.m[3] * matrixRight.m[4]  + matrixLeft.m[7] * matrixRight.m[5]  + matrixLeft.m[11] * matrixRight.m[6]  + matrixLeft.m[15] * matrixRight.m[7];
	m.m[11] = matrixLeft.m[3] * matrixRight.m[8]  + matrixLeft.m[7] * matrixRight.m[9]  + matrixLeft.m[11] * matrixRight.m[10] + matrixLeft.m[15] * matrixRight.m[11];
	m.m[15] = matrixLeft.m[3] * matrixRight.m[12] + matrixLeft.m[7] * matrixRight.m[13] + matrixLeft.m[11] * matrixRight.m[14] + matrixLeft.m[15] * matrixRight.m[15];
    return m;
}

Matrix4 MatrixInvert(Matrix4 matrix) {
    Matrix4 m;
    Matrix4 res;
    
    m.m[0] = matrix.m[5]  * matrix.m[10] * matrix.m[15] -
             matrix.m[5]  * matrix.m[11] * matrix.m[14] -
             matrix.m[9]  * matrix.m[6]  * matrix.m[15] +
             matrix.m[9]  * matrix.m[7]  * matrix.m[14] +
             matrix.m[13] * matrix.m[6]  * matrix.m[11] -
             matrix.m[13] * matrix.m[7]  * matrix.m[10];
    
    m.m[4] = -matrix.m[4]  * matrix.m[10] * matrix.m[15] +
             matrix.m[4]  * matrix.m[11] * matrix.m[14] +
             matrix.m[8]  * matrix.m[6]  * matrix.m[15] -
             matrix.m[8]  * matrix.m[7]  * matrix.m[14] -
             matrix.m[12] * matrix.m[6]  * matrix.m[11] +
             matrix.m[12] * matrix.m[7]  * matrix.m[10];
    
    m.m[8] = matrix.m[4]  * matrix.m[9] * matrix.m[15] -
             matrix.m[4]  * matrix.m[11] * matrix.m[13] -
             matrix.m[8]  * matrix.m[5] * matrix.m[15] +
             matrix.m[8]  * matrix.m[7] * matrix.m[13] +
             matrix.m[12] * matrix.m[5] * matrix.m[11] -
             matrix.m[12] * matrix.m[7] * matrix.m[9];
    
    m.m[12] = -matrix.m[4]  * matrix.m[9] * matrix.m[14] +
             matrix.m[4]  * matrix.m[10] * matrix.m[13] +
             matrix.m[8]  * matrix.m[5] * matrix.m[14] -
             matrix.m[8]  * matrix.m[6] * matrix.m[13] -
             matrix.m[12] * matrix.m[5] * matrix.m[10] +
             matrix.m[12] * matrix.m[6] * matrix.m[9];
    
    m.m[1] = -matrix.m[1]  * matrix.m[10] * matrix.m[15] +
             matrix.m[1]  * matrix.m[11] * matrix.m[14] +
             matrix.m[9]  * matrix.m[2] * matrix.m[15] -
             matrix.m[9]  * matrix.m[3] * matrix.m[14] -
             matrix.m[13] * matrix.m[2] * matrix.m[11] +
             matrix.m[13] * matrix.m[3] * matrix.m[10];
    
    m.m[5] = matrix.m[0]  * matrix.m[10] * matrix.m[15] -
             matrix.m[0]  * matrix.m[11] * matrix.m[14] -
             matrix.m[8]  * matrix.m[2] * matrix.m[15] +
             matrix.m[8]  * matrix.m[3] * matrix.m[14] +
             matrix.m[12] * matrix.m[2] * matrix.m[11] -
             matrix.m[12] * matrix.m[3] * matrix.m[10];
    
    m.m[9] = -matrix.m[0]  * matrix.m[9] * matrix.m[15] +
             matrix.m[0]  * matrix.m[11] * matrix.m[13] +
             matrix.m[8]  * matrix.m[1] * matrix.m[15] -
             matrix.m[8]  * matrix.m[3] * matrix.m[13] -
             matrix.m[12] * matrix.m[1] * matrix.m[11] +
             matrix.m[12] * matrix.m[3] * matrix.m[9];
    
    m.m[13] = matrix.m[0]  * matrix.m[9] * matrix.m[14] -
             matrix.m[0]  * matrix.m[10] * matrix.m[13] -
             matrix.m[8]  * matrix.m[1] * matrix.m[14] +
             matrix.m[8]  * matrix.m[2] * matrix.m[13] +
             matrix.m[12] * matrix.m[1] * matrix.m[10] -
             matrix.m[12] * matrix.m[2] * matrix.m[9];
    
    m.m[2] = matrix.m[1]  * matrix.m[6] * matrix.m[15] -
             matrix.m[1]  * matrix.m[7] * matrix.m[14] -
             matrix.m[5]  * matrix.m[2] * matrix.m[15] +
             matrix.m[5]  * matrix.m[3] * matrix.m[14] +
             matrix.m[13] * matrix.m[2] * matrix.m[7] -
             matrix.m[13] * matrix.m[3] * matrix.m[6];
    
    m.m[6] = -matrix.m[0]  * matrix.m[6] * matrix.m[15] +
             matrix.m[0]  * matrix.m[7] * matrix.m[14] +
             matrix.m[4]  * matrix.m[2] * matrix.m[15] -
             matrix.m[4]  * matrix.m[3] * matrix.m[14] -
             matrix.m[12] * matrix.m[2] * matrix.m[7] +
             matrix.m[12] * matrix.m[3] * matrix.m[6];
    
    m.m[10] = matrix.m[0]  * matrix.m[5] * matrix.m[15] -
             matrix.m[0]  * matrix.m[7] * matrix.m[13] -
             matrix.m[4]  * matrix.m[1] * matrix.m[15] +
             matrix.m[4]  * matrix.m[3] * matrix.m[13] +
             matrix.m[12] * matrix.m[1] * matrix.m[7] -
             matrix.m[12] * matrix.m[3] * matrix.m[5];
    
    m.m[14] = -matrix.m[0]  * matrix.m[5] * matrix.m[14] +
             matrix.m[0]  * matrix.m[6] * matrix.m[13] +
             matrix.m[4]  * matrix.m[1] * matrix.m[14] -
             matrix.m[4]  * matrix.m[2] * matrix.m[13] -
             matrix.m[12] * matrix.m[1] * matrix.m[6] +
             matrix.m[12] * matrix.m[2] * matrix.m[5];
    
    m.m[3] = -matrix.m[1] * matrix.m[6] * matrix.m[11] +
             matrix.m[1] * matrix.m[7] * matrix.m[10] +
             matrix.m[5] * matrix.m[2] * matrix.m[11] -
             matrix.m[5] * matrix.m[3] * matrix.m[10] -
             matrix.m[9] * matrix.m[2] * matrix.m[7] +
             matrix.m[9] * matrix.m[3] * matrix.m[6];
    
    m.m[7] = matrix.m[0] * matrix.m[6] * matrix.m[11] -
             matrix.m[0] * matrix.m[7] * matrix.m[10] -
             matrix.m[4] * matrix.m[2] * matrix.m[11] +
             matrix.m[4] * matrix.m[3] * matrix.m[10] +
             matrix.m[8] * matrix.m[2] * matrix.m[7] -
             matrix.m[8] * matrix.m[3] * matrix.m[6];
    
    m.m[11] = -matrix.m[0] * matrix.m[5] * matrix.m[11] +
             matrix.m[0] * matrix.m[7] * matrix.m[9] +
             matrix.m[4] * matrix.m[1] * matrix.m[11] -
             matrix.m[4] * matrix.m[3] * matrix.m[9] -
             matrix.m[8] * matrix.m[1] * matrix.m[7] +
             matrix.m[8] * matrix.m[3] * matrix.m[5];
    
    m.m[15] = matrix.m[0] * matrix.m[5] * matrix.m[10] -
              matrix.m[0] * matrix.m[6] * matrix.m[9] -
              matrix.m[4] * matrix.m[1] * matrix.m[10] +
              matrix.m[4] * matrix.m[2] * matrix.m[9] +
              matrix.m[8] * matrix.m[1] * matrix.m[6] -
              matrix.m[8] * matrix.m[2] * matrix.m[5];
    
    float det = matrix.m[0] * m.m[0] + matrix.m[1] * m.m[4] + matrix.m[2] * m.m[8] + matrix.m[3] * m.m[12];
    
    if (det == 0)
        return matrix;
    
    det = 1.0f / det;
    
    for (int i = 0; i < 16; i++) {
        res.m[i] = m.m[i] * det;
    }
    
    return res;
}

Matrix4 MatrixTranspose(Matrix4 matrix) {
    Matrix4 res = { matrix.m[0], matrix.m[4], matrix.m[8], matrix.m[12],
            matrix.m[1], matrix.m[5], matrix.m[9], matrix.m[13],
            matrix.m[2], matrix.m[6], matrix.m[10], matrix.m[14],
            matrix.m[3], matrix.m[7], matrix.m[11], matrix.m[15]
          };
    return res;
}

Matrix4 MatrixMakeOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    Matrix4 res = { 2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -2.0f / (farPlane - nearPlane), 0.0f,
            -(right + left) / (right - left), -(top + bottom) / (top - bottom),
            -(farPlane + nearPlane) / (farPlane - nearPlane), 1.0f
    };
    
    return res;
}

Matrix4 MatrixMakePerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
    float cotan = 1.0f / tanf(fov / 2.0f * M_PI / 180.0f);
	
	Matrix4 res = { cotan / aspectRatio, 0.0f, 0.0f, 0.0f,
            0.0f, cotan, 0.0f, 0.0f,
            0.0f, 0.0f, (farPlane + nearPlane) / (nearPlane - farPlane), -1.0f,
            0.0f, 0.0f, (2.0f * farPlane * nearPlane) / (nearPlane - farPlane), 0.0f
          };
    
    return res;
}

Matrix4 MatrixMakeLookAt(Vector3 eye, Vector3 center, Vector3 up) {
        
	Vector3 n = (eye - center).Normalize();
	Vector3 u = up.Cross(n).Normalize();
	Vector3 v = n.Cross(u);
	
	Matrix4 res = { u.x, v.x, n.x, 0.0f,
            u.y, v.y, n.y, 0.0f,
            u.z, v.z, n.z, 0.0f,
            (-u).Dot(eye),
            (-v).Dot(eye),
            (-n).Dot(eye),
            1.0f
          };
    
    return res;
}

Matrix4 MatrixIdentity() {
    Matrix4 mat = { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
    return mat;
}

Matrix4 MatrixBias() {
    Matrix4 mat = { 0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f
    };
    return mat;
}

Matrix4 MatrixTranslate(Vector3 translation) {
    Matrix4 mat = MatrixIdentity();
    mat.m[12] = translation.x;
    mat.m[13] = translation.y;
    mat.m[14] = translation.z;
    return mat;
}

Matrix4 MatrixScale(float x, float y, float z) {
    Matrix4 mat = MatrixIdentity();
    mat.m[0] = x;
    mat.m[5] = y;
    mat.m[10] = z;
    return mat;
}

Matrix4 MatrixMakeRotationY(float rotation) {
    float rad = rotation * M_PI / 180.0f;
    float cos = cosf(rad);
    float sin = sinf(rad);
    Matrix4 mat = { cos, 0.0f, -sin, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sin, 0.0f, cos, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
          };
    return mat;
}

Matrix4 MatrixMakeRotationX(float rotation) {
    float rad = rotation * M_PI / 180.0f;
    float cos = cosf(rad);
    float sin = sinf(rad);
    Matrix4 mat = { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos, sin, 0.0f,
            0.0f, -sin, cos, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
          };
    return mat;
}

Matrix4 MatrixMakeRotationZ(float rotation) {
    float rad = rotation * M_PI / 180.0f;
    float cos = cosf(rad);
    float sin = sinf(rad);
    Matrix4 mat = { cos, sin, 0.0f, 0.0f,
            -sin, cos, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
          };
    return mat;
}



Matrix4 MatrixScale(Vector3 scale) {
	return MatrixScale(scale.x, scale.y, scale.z);
}