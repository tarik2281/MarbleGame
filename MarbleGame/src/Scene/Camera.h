//
//  Camera.h
//  IndoorEditor
//
//  Created by Tarik Karaca on 14.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __IndoorEditor__Camera__
#define __IndoorEditor__Camera__

#include "Maths/VectorMath.h"
#include "Maths/MatrixMath.h"

class Camera {
public:
    virtual void setRotationX(float);
    virtual void setRotationY(float);
    virtual void setPosition(const Vector3&);
    
    void setProjection(const Matrix4&);
    
    Matrix4& getProjection();
    Matrix4& getView();
    
    virtual Vector3 getPosition();
    
    Camera();
protected:
    Vector3 rotation;
    Vector3 position;
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
};

class LookAtCamera : public Camera {
public:
    void addRotationX(float);
    void addRotationY(float);
    void addPosition(const Vector3&);
    void setRotationX(float);
    void setRotationY(float);
    float getRotationX();
    float getRotationY();
    void setPosition(const Vector3&);
    
    void setZoom(float);
    void addZoom(float);
    float getZoom();
    
    Vector3 getPosition();
    
    LookAtCamera();
private:
    void updateCamera();
    
    float zoom;
};

#endif /* defined(__IndoorEditor__Camera__) */
