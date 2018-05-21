//
//  Shader.h
//  TerrainEditing
//
//  Created by Tarik Karaca on 26.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __TerrainEditing__Shader__
#define __TerrainEditing__Shader__

//#define DeleteShader(shader) shader->release(); delete shader; shader = 0;

#include "GeometryMath.h"
#include "Resource.h"


#define MODEL_SHADER_LOCATION 1
#define SHADOW_SHADER_LOCATION 2
#define VBLUR_SHADER_LOCATION 3
#define BILLBOARD_SHADER_LOCATION 4

enum ShaderType {
    ShaderTypeModel,
    ShaderTypeShadow,
    ShaderTypeBlur,
    ShaderTypesLength
};

class Shader : public IResource {
public:
    enum Values {
        ValuesModelMatrix,
        ValuesDiffuseMapSampler,
        ValuesShadowMapSampler,
        ValuesShadowProjectionMatrix,
        ValuesShadowViewMatrix,
        ValuesBiasMatrix,
        ValuesCameraPosition,
        ValuesLength
    };
    
    virtual int getUniformLocation(int uniform) = 0;
    virtual void use();
    
    template<class T>
    static void setUniform(int index, T value);
    
    template<class T>
    static void setValue(int index, T value);
    
    void LoadResource(const char* path);
    void Release();
    
    virtual int getShaderType() = 0;
    
    static void Unbind();
protected:
    unsigned int program;
    
    int values[ValuesLength];
    
    static Shader* currentShader; // reference only
};

/*class ModelShader : public Shader {
public:
    enum Uniforms {
        UniformsTexSampler,
        UniformsModelMatrix,
        UniformsRotationMatrix,
        UniformsAlpha,
        UniformsUseAlpha,
        UniformsWaterHeight,
        UniformsNormalMapSampler,
        UniformsShadowMap,
        UniformsShadowViewMatrix,
        UniformsShadowProjectionMatrix,
        UniformsBiasMatrix,
        UniformCamPos,
        UniformsLength
    };
    
    int getUniformLocation(int uniform);
    
    ModelShader() { }
    
    int getShaderType();
    
    void LoadResource(const char* path);
    
    static ModelShader* loadShader(const char* vertexSource,
                                   const char* fragmentSource);
private:
    int uniforms[UniformsLength];
};*/

class ShadowShader: public Shader {
public:
    enum Uniforms {
        UniformsLength
    };
    
    int getUniformLocation(int uniform);
    
    ShadowShader() { }
    
    int getShaderType();
    
    void LoadResource(const char* path);
    
private:
    int uniforms[UniformsLength];
};

class BlurShader : public Shader {
public:
    enum Uniforms {
        UniformsBlurDirection,
        UniformsScreenSize,
        UniformsLength
    };
    
    int getUniformLocation(int uniform);
    
    BlurShader() { }
    
    int getShaderType();
    
    void LoadResource(const char* path);
private:
    int uniforms[UniformsLength];
};


#include "Util.h"
//#include <iostream>

inline void Shader::Unbind() {
    currentShader = 0;
    glUseProgram(0);
}

inline void Shader::use() {
    if (Shader::currentShader != this) {
        Shader::currentShader = this;
        glUseProgram(this->program);
    } else {
        //printf("already bound: %d\n", this->program);
    }
}

template<>
inline void Shader::setUniform<Matrix4>(int index, Matrix4 value) {
    if (!currentShader)
        return;
    int loc = currentShader->getUniformLocation(index);
    if (loc == -1)
        return;
    glUniformMatrix4fv(loc, 1, false, value.m);
}

template<>
inline void Shader::setUniform<Vector2>(int index, Vector2 value) {
    if (!currentShader)
    return;
    int loc = currentShader->getUniformLocation(index);
    if (loc == -1)
    return;
    glUniform2fv(loc, 1, value.v);
}

template<>
inline void Shader::setUniform<Vector3>(int index, Vector3 value) {
    if (!currentShader)
        return;
    int loc = currentShader->getUniformLocation(index);
    if (loc == -1)
        return;
    glUniform3fv(loc, 1, value.v);
}

template<>
inline void Shader::setUniform<float>(int index, float value) {
    if (!currentShader)
        return;
    int loc = currentShader->getUniformLocation(index);
    if (loc == -1)
        return;
    glUniform1f(loc, value);
}

template<>
inline void Shader::setUniform<int>(int index, int value) {
    if (!currentShader)
        return;
    int loc = currentShader->getUniformLocation(index);
    if (loc == -1)
        return;
    glUniform1i(loc, value);
}

template<>
inline void Shader::setValue(int index, Matrix4 value) {
    if (!currentShader)
        return;
    int loc = currentShader->values[index];
    if (loc == -1)
        return;
    glUniformMatrix4fv(loc, 1, false, value.m);
}

template<>
inline void Shader::setValue(int index, Vector3 value) {
    if (!currentShader)
        return;
    int loc = currentShader->values[index];
    if (loc == -1)
        return;
    glUniform3fv(loc, 1, value.v);
}

template<>
inline void Shader::setValue(int index, int value) {
    if (!currentShader)
        return;
    int loc = currentShader->values[index];
    if (loc == -1)
        return;
    glUniform1i(loc, value);
}

#endif /* defined(__TerrainEditing__Shader__) */
