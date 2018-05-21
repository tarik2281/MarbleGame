//
//  PhysicsDebugDrawer.cpp
//  Terrain
//
//  Created by Tarik Karaca on 22.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "PhysicsDebugDrawer.h"
#include "Util.h"
#include "Shader.h"

void PhysicsDebugDrawer::drawLine(const btVector3 & from, const btVector3& to, const btVector3& color) {
    glColor3f(color.x() * 5, color.y() * 5, color.z() * 5);
    Shader::Unbind();
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(from.x(), from.y(), from.z());
    glVertex3f(to.x(), to.y(), to.z());
    glEnd();
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB,
                                          btScalar distance, int lifeTime, const btVector3 &color) {
    
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString) {
    printf("%s\n", warningString);
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString) {
    
}

void PhysicsDebugDrawer::setDebugMode(int debugMode) {
    
}

int PhysicsDebugDrawer::getDebugMode() const {
    return DBG_DrawWireframe | DBG_DrawAabb;
}