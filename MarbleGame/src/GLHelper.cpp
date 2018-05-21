//
//  GLHelper.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "GLHelper.h"

#include "Util.h"

void drawBufferIndexedVertices(unsigned int size) {
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, (void*)0);
}