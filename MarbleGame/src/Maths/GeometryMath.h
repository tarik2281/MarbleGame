//
//  GeometryMath.h
//  Editor
//
//  Created by Tarik Karaca on 10.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __Editor__GeometryMath__
#define __Editor__GeometryMath__

#include "VectorMath.h"
#include "MatrixMath.h"
#include "QuaternionMath.h"

struct Ray {
    Vector3 position;
    Vector3 direction;
};

struct Plane {
    Vector3 normal;
    Vector3 point; // mostly origin
};

struct Frustum {
	Vector3 origin;
	Quaternion orientation;

	float leftSlope;
	float rightSlope;
	float topSlope;
	float bottomSlope;
	float nearPlane;
	float farPlane;
};

enum PlaneType {
    PlaneNone,
    PlaneLeft,
    PlaneRight,
    PlaneTop,
    PlaneBottom,
    PlaneFront,
    PlaneBack
};

struct BoundingBox {
    Vector3 origin;
    Vector3 halfSize;
    
    Vector3 Min();
    Vector3 Max();
};

float RayDistanceToPlane(const Ray& ray, const Plane& plane, bool* isFacing = 0);
PlaneType RayIntersectsBoundingBox(const Ray& ray, const BoundingBox& box, float* distance = 0);

Frustum FrustumFromMatrix(const Matrix4&);
void CornerPointsFromFrustum(Frustum, Vector4*, float pNear, float pFar);
void CornerPointsFromAABB(const Vector3& min, const Vector3& max, Vector4*);

bool BoundingBoxContainsPoint(BoundingBox box1, Vector3 point);
bool BoundingBoxesIntersect(BoundingBox box1, BoundingBox box2, bool* intersectTop = 0);

void debugRenderRay(const Ray&);
void debugRenderBoundingBox(const BoundingBox&);

void Unproject(Vector3 winPos, Matrix4 projMat, Matrix4 viewMat, int* viewport, Vector3* outWorldPos);

inline Vector3 BoundingBox::Min() {
    return origin - halfSize;
}

inline Vector3 BoundingBox::Max() {
    return origin + halfSize;
}

#endif /* defined(__Editor__GeometryMath__) */
