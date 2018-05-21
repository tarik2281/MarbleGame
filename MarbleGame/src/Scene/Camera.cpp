//
//  Camera.cpp
//  IndoorEditor
//
//  Created by Tarik Karaca on 14.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "Camera.h"
#include "Maths/QuaternionMath.h"

void Camera::setRotationX(float rotation) {
    this->rotation.x = rotation;
}

void Camera::setRotationY(float rotation) {
    this->rotation.y = rotation;
    
    viewMatrix = MatrixTranslate(position);
}

void Camera::setPosition(const Vector3& position) {
    this->position = position;
    
    viewMatrix =  MatrixTranslate(position);
}

void Camera::setProjection(const Matrix4 &m) {
    this->projectionMatrix = m;
}

Matrix4& Camera::getProjection() {
    return this->projectionMatrix;
}

Matrix4& Camera::getView() {
    return this->viewMatrix;
}

Vector3 Camera::getPosition() {
    return position;
}

Camera::Camera() {
    this->position = VectorZero;
    this->rotation = VectorZero;
    
    viewMatrix = MatrixIdentity();
    projectionMatrix = MatrixIdentity();
}


void LookAtCamera::addRotationX(float rotation) {
    this->rotation.x += rotation;
    if (this->rotation.x > 89)
        this->rotation.x = 89;
    
    if (this->rotation.x < -89)
        this->rotation.x = -89;
    
    updateCamera();
}

void LookAtCamera::addRotationY(float rotation) {
    this->rotation.y += rotation;
    
    updateCamera();
}

void LookAtCamera::addPosition(const Vector3 &position) {
    this->position = this->position + (Vector3)position;
    
    updateCamera();
}

void LookAtCamera::setRotationX(float rotation) {
    this->rotation.x = rotation;
    
    updateCamera();
}

void LookAtCamera::setRotationY(float rotation) {
    this->rotation.y = rotation;
    
    updateCamera();
}

void LookAtCamera::setPosition(const Vector3 &position) {
    this->position = position;
    
    updateCamera();
}

void LookAtCamera::setZoom(float zoom) {
    this->zoom = zoom;
    
    updateCamera();
}

void LookAtCamera::addZoom(float zoom) {
    this->zoom += zoom;
    
    updateCamera();
}

float LookAtCamera::getZoom() {
    return this->zoom;
}

Vector3 LookAtCamera::getPosition() {
    Quaternion rot = Quaternion(Vector3(0.0f, 1.0f, 0.0f), rotation.y) * Quaternion(Vector3(1.0f, 0.0, 0.0f), rotation.x);
    Vector3 res = rot * Vector3(0.0f, 0.0f, zoom);
    return res + position;
}

void LookAtCamera::updateCamera() {
    //Vector3 res = (MatrixMakeRotationY(rotation.y) * MatrixMakeRotationX(rotation.x)) *
      //              Vector3(0.0f, 0.0f, zoom);
	Quaternion rot = Quaternion(Vector3(0.0f, 1.0f, 0.0f), rotation.y) * Quaternion(Vector3(1.0f, 0.0, 0.0f), rotation.x);
	Vector3 res = rot * Vector3(0.0f, 0.0f, zoom);
	viewMatrix = MatrixMakeLookAt(res + position, position, VectorUp);
}

LookAtCamera::LookAtCamera() {
    this->position = VectorZero;
    this->rotation = VectorZero;
    
    viewMatrix = MatrixIdentity();
    projectionMatrix = MatrixIdentity();
    
    zoom = 0.0f;
}

float LookAtCamera::getRotationX() {
    return this->rotation.x;
}

float LookAtCamera::getRotationY() {
    return this->rotation.y;
}
