//
//  GeometryMath.cpp
//  Editor
//
//  Created by Tarik Karaca on 10.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "GeometryMath.h"
//#include "Renderer.h"
//#include "Shader.h"
#include "../Util.h"

float RayDistanceToPlane(const Ray& ray, const Plane& plane, bool* isFacing) {
    float den = (plane.point - ray.position).Dot(plane.normal);
    float num = ray.direction.Dot(plane.normal);
    if (isFacing)
        *isFacing = (num < 0.0);
    return den / num;
}

PlaneType RayIntersectsBoundingBox(const Ray& ray, const BoundingBox& box, float* distance) {
    Plane plane;
    float d;
    bool isFacing = false;
    Vector3 endRay;
    plane.point = Vector3(box.origin.x - box.halfSize.x, box.origin.y, box.origin.z);
    plane.normal = Vector3(-1, 0, 0);
    d = RayDistanceToPlane(ray, plane, &isFacing);
    endRay = ray.position + ray.direction * d;
    if (endRay.z < box.origin.z + box.halfSize.z && endRay.z > box.origin.z - box.halfSize.z &&
        endRay.y < box.origin.y + box.halfSize.y && endRay.y > box.origin.y -box.halfSize.y && isFacing) {
        if (distance)
            *distance = d;
        return PlaneLeft;
    }
    
    plane.point = Vector3(box.origin.x + box.halfSize.x, box.origin.y, box.origin.z);
    plane.normal = Vector3(1, 0, 0);
    d = RayDistanceToPlane(ray, plane, &isFacing);
    endRay = ray.position + ray.direction * d;
    if (endRay.z < box.origin.z + box.halfSize.z && endRay.z > box.origin.z - box.halfSize.z &&
        endRay.y < box.origin.y + box.halfSize.y && endRay.y > box.origin.y -box.halfSize.y && isFacing) {
        if (distance)
            *distance = d;
        return PlaneRight;
    }
    
    plane.point = Vector3(box.origin.x, box.origin.y + box.halfSize.y, box.origin.z);
    plane.normal = Vector3(0, 1, 0);
    d = RayDistanceToPlane(ray, plane, &isFacing);
    endRay = ray.position + ray.direction * d;
    if (endRay.z < box.origin.z + box.halfSize.z && endRay.z > box.origin.z - box.halfSize.z &&
        endRay.x < box.origin.x + box.halfSize.x && endRay.x > box.origin.x -box.halfSize.x && isFacing) {
        if (distance)
            *distance = d;
        return PlaneTop;
    }
    
    plane.point = Vector3(box.origin.x, box.origin.y - box.halfSize.y, box.origin.z);
    plane.normal = Vector3(0, -1, 0);
    d = RayDistanceToPlane(ray, plane, &isFacing);
    endRay = ray.position + ray.direction * d;
    if (endRay.z < box.origin.z + box.halfSize.z && endRay.z > box.origin.z - box.halfSize.z &&
        endRay.x < box.origin.x + box.halfSize.x && endRay.x > box.origin.x -box.halfSize.x && isFacing) {
        if (distance)
            *distance = d;
        return PlaneBottom;
    }
    
    plane.point = Vector3(box.origin.x, box.origin.y, box.origin.z + box.halfSize.z);
    plane.normal = Vector3(0, 0, 1);
    d = RayDistanceToPlane(ray, plane, &isFacing);
    endRay = ray.position + ray.direction * d;
    if (endRay.y < box.origin.y + box.halfSize.y && endRay.y > box.origin.y - box.halfSize.y &&
        endRay.x < box.origin.x + box.halfSize.x && endRay.x > box.origin.x -box.halfSize.x && isFacing) {
        if (distance)
            *distance = d;
        return PlaneFront;
    }
    
    plane.point = Vector3(box.origin.x, box.origin.y, box.origin.z - box.halfSize.z);
    plane.normal = Vector3(0, 0, -1);
    d = RayDistanceToPlane(ray, plane, &isFacing);
    endRay = ray.position + ray.direction * d;
    if (endRay.y < box.origin.y + box.halfSize.y && endRay.y > box.origin.y - box.halfSize.y &&
        endRay.x < box.origin.x + box.halfSize.x && endRay.x > box.origin.x -box.halfSize.x && isFacing) {
        if (distance)
            *distance = d;
        return PlaneBack;
    }
    
    return PlaneNone;
}

Frustum FrustumFromMatrix(const Matrix4& mProjection) {
	Frustum frustum;
	
	Vector4 homogeneousPoints[6] = {
		Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(-1.0f, 0.0f, 1.0f, 1.0f),
		Vector4(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4(0.0f, -1.0f, 1.0f, 1.0f),
		Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		Vector4(0.0f, 0.0f, 1.0f, 1.0f)
	};

	Matrix4 invProjection = MatrixInvert(mProjection);
	Vector4 Points[6];

	for (int i = 0; i < 6; i++) {
		Points[i] = invProjection * homogeneousPoints[i];

		if (i < 4) {
			Points[i] /= Points[i].z;
		}
		else {
			Points[i] /= Points[i].w;
		}
	}

	frustum.origin = Vector3(0.0f);
	frustum.orientation = QUATERNION_IDENTITY;

	frustum.rightSlope = Points[0].x;
	frustum.leftSlope = Points[1].x;
	frustum.topSlope = Points[2].y;
	frustum.bottomSlope = Points[3].y;

	frustum.nearPlane = Points[4].z;
	frustum.farPlane = Points[5].z;

	return frustum;
}

void CornerPointsFromFrustum(Frustum frustum, Vector4* outPoints, float pNear, float pFar) {
	frustum.nearPlane = -pNear;
	frustum.farPlane = -pFar;

	outPoints[0] = Vector4(frustum.rightSlope * frustum.nearPlane, frustum.topSlope * frustum.nearPlane, frustum.nearPlane, 1.0f);
	outPoints[1] = Vector4(frustum.leftSlope * frustum.nearPlane, frustum.topSlope * frustum.nearPlane, frustum.nearPlane, 1.0f);
	outPoints[2] = Vector4(frustum.leftSlope * frustum.nearPlane, frustum.bottomSlope * frustum.nearPlane, frustum.nearPlane, 1.0f);
	outPoints[3] = Vector4(frustum.rightSlope * frustum.nearPlane, frustum.bottomSlope * frustum.nearPlane, frustum.nearPlane, 1.0f);

	outPoints[4] = Vector4(frustum.rightSlope * frustum.farPlane, frustum.topSlope * frustum.farPlane, frustum.farPlane, 1.0f);
	outPoints[5] = Vector4(frustum.leftSlope * frustum.farPlane, frustum.topSlope * frustum.farPlane, frustum.farPlane, 1.0f);
	outPoints[6] = Vector4(frustum.leftSlope * frustum.farPlane, frustum.bottomSlope * frustum.farPlane, frustum.farPlane, 1.0f);
	outPoints[7] = Vector4(frustum.rightSlope * frustum.farPlane, frustum.bottomSlope * frustum.farPlane, frustum.farPlane, 1.0f);
}

void CornerPointsFromAABB(const Vector3& min, const Vector3& max, Vector4* outPoints) {
	outPoints[0] = Vector4(min, 1.0f);
	outPoints[1] = Vector4(min.x, min.y, max.z, 1.0f);
	outPoints[2] = Vector4(min.x, max.y, min.z, 1.0f);
	outPoints[3] = Vector4(max.x, min.y, min.z, 1.0f);
	outPoints[4] = Vector4(min.x, max.y, max.z, 1.0f);
	outPoints[5] = Vector4(max.x, max.y, min.z, 1.0f);
	outPoints[6] = Vector4(max.x, min.y, max.z, 1.0f);
	outPoints[7] = Vector4(max, 1.0f);
}

bool BoundingBoxContainsPoint(BoundingBox box1, Vector3 point) {
    Vector3 min = box1.origin - box1.halfSize;
    Vector3 max = box1.origin + box1.halfSize;
    
    if (point.x > min.x && point.x < max.x && point.y > min.y && point.y < max.y &&
        point.z > min.z && point.z < max.z)
    return true;
    
    return false;
}

bool BoundingBoxesIntersect(BoundingBox box1, BoundingBox box2, bool* intersectTop) {
    float planeX1 = box1.origin.x - box1.halfSize.x;
    float planeX2 = box1.origin.x + box1.halfSize.x;
    float planeY1 = box1.origin.y - box1.halfSize.y;
    float planeY2 = box1.origin.y + box1.halfSize.y;
    float planeZ1 = box1.origin.z - box1.halfSize.z;
    float planeZ2 = box1.origin.z + box1.halfSize.z;
    
    if (planeX1 > box2.origin.x + box2.halfSize.x) return false;
    if (planeX2 < box2.origin.x - box2.halfSize.x) return false;
    if (planeY1 > box2.origin.y + box2.halfSize.y) return false;
    if (planeY2 < box2.origin.y - box2.halfSize.y) return false;
    if (planeZ1 > box2.origin.z + box2.halfSize.z) return false;
    if (planeZ2 < box2.origin.z - box2.halfSize.z) return false;
    
    if (intersectTop) {
        *intersectTop = false;
        if (planeY2 > box2.origin.y - box2.halfSize.y) {
            *intersectTop = true;
        }
    }

    return true;
}

void debugRenderBoundingBox(const BoundingBox& box) {
	/*
	//gridShader->use();
    glLineWidth(1.0f);
    Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);
    //glUniform4fv(gridShader->getUniformLocation(GridShader::UniformsColor), 1, &color.x);
    
    glBegin(GL_LINES);
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z - box.halfSize.z);
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z + box.halfSize.z);
    
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z - box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z + box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z + box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z + box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z - box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z - box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x + box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y - box.halfSize.y, box.origin.z - box.halfSize.z);
    
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z + box.halfSize.z);
    glVertex3f(box.origin.x - box.halfSize.x, box.origin.y + box.halfSize.y, box.origin.z - box.halfSize.z);

    glEnd();*/
}

void Unproject(Vector3 winPos, Matrix4 projMat, Matrix4 viewMat, int* viewport, Vector3* outWorldPos) {
    Matrix4 A = projMat * viewMat;
    Matrix4 m = MatrixInvert(A);
    Vector4 _in, _out;
    
    _in.x = (winPos.x - (float)viewport[0])/(float)viewport[2] * 2.0f - 1.0f;
    _in.y = (winPos.y - (float)viewport[1])/(float)viewport[3] * 2.0f - 1.0f;
    _in.z = 2.0f * winPos.z - 1.0f;
    _in.w = 1.0f;
    
    _out = m * _in;
    outWorldPos->x = _out.x / _out.w;
    outWorldPos->y = _out.y / _out.w;
    outWorldPos->z = _out.z / _out.w;
}