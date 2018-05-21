//
//  PhysicsDebugDrawer.h
//  Terrain
//
//  Created by Tarik Karaca on 22.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __Terrain__PhysicsDebugDrawer__
#define __Terrain__PhysicsDebugDrawer__

#include <LinearMath/btIDebugDraw.h>

class PhysicsDebugDrawer : public btIDebugDraw {
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB,
                                btScalar distance, int lifeTime, const btVector3 &color);
    void reportErrorWarning(const char* warningString);
    void draw3dText(const btVector3 & location, const char* textString);
    void setDebugMode(int debugMode);
    int getDebugMode() const;
};

#endif /* defined(__Terrain__PhysicsDebugDrawer__) */
