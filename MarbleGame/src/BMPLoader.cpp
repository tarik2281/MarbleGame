//
//  BMPLoader.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 03.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

/*#include "BMPLoader.h"

#include <stdio.h>

#include "Util.h"

unsigned int LoadBMP(const char* path) {
    FILE* fp = fopen(path, "rb");
    
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;
    
    fread(&header, 1, sizeof(BITMAPFILEHEADER), fp);
    fread(&info, 1, sizeof(BITMAPINFOHEADER), fp);
    
    unsigned char* pixels = (unsigned char*)malloc(info.biWidth * info.biHeight * info.biBitCount / 8);
    
    fseek(fp, header.btOffBits, SEEK_SET);
    
    fread(pixels, 1, info.biSizeImage, fp);
    
    fclose(fp);
    
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.biWidth, info.biHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    free(pixels);
    
    return texName;
}*/