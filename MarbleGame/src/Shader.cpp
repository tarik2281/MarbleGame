//
//  Shader.cpp
//  TerrainEditing
//
//  Created by Tarik Karaca on 26.11.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "Shader.h"
#include <fstream>


Shader* Shader::currentShader = 0;

GLuint createShader(const char* shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    
    glShaderSource(shader, 1, &shaderSource, 0);
    glCompileShader(shader);
    
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    
    if (!result) {
        int infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char errorMessage[infoLogLength];
        glGetShaderInfoLog(shader, infoLogLength, 0, errorMessage);
        fprintf(stderr, "%s\n", errorMessage);
    }
    
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint program = glCreateProgram();
    
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    
    if (vertexSource != 0) {
        vertexShader = createShader(vertexSource, GL_VERTEX_SHADER);
        glAttachShader(program, vertexShader);
    }
    
    if (fragmentSource != 0) {
        GLuint fragmentShader = createShader(fragmentSource, GL_FRAGMENT_SHADER);
        glAttachShader(program, fragmentShader);
    }
    
    glLinkProgram(program);
    
    if (vertexShader)
        glDeleteShader(vertexShader);
    
    if (fragmentShader)
        glDeleteShader(fragmentShader);
    
    return program;
}

#define ToNSString(string) [NSString stringWithCString:string encoding:NSUTF8StringEncoding]

void Shader::LoadResource(const char *path) {
    std::string vPath = std::string(path) + ".vsh";
    std::string fPath = std::string(path) + ".fsh";
    
    std::ifstream file;
    file.open(vPath.c_str());
    std::string vSource;
    
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            vSource += line + '\n';
        }
    }
    
    file.close();
    
    file.open(fPath.c_str());
    std::string fSource;
    
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            fSource += line + '\n';
        }
    }
    
    file.close();
    
    //NSString* vSource = [NSString stringWithContentsOfFile:ToNSString(vPath.c_str()) encoding:NSUTF8StringEncoding error:nil];
    //NSString* fSource = [NSString stringWithContentsOfFile:ToNSString(fPath.c_str()) encoding:NSUTF8StringEncoding error:nil];
    program = createProgram(vSource.c_str(), fSource.c_str());
    
    const char* valueNames[ValuesLength] = {
        "modelMatrix", "diffuseMapSampler", "shadowMapSampler", "shadowProjectionMatrix", "shadowViewMatrix",
        "biasMatrix", "cameraPosition"
    };
    
    for (int i = 0; i < ValuesLength; i++) {
        values[i] = glGetUniformLocation(program, valueNames[i]);
    }
}

void Shader::Release() {
    glDeleteProgram(this->program);
}

void ModelShader::LoadResource(const char *path) {
    Shader::LoadResource(path);
    
    const char* uniformNames[ModelShader::UniformsLength] = {
        "texSampler", "modelMatrix", "rotMatrix", "alpha", "useAlpha",
        "waterHeight", "normalMapSampler", "shadowMap", "shadowViewMatrix",
        "shadowProjectionMatrix", "biasMatrix", "camPos"
    };
    
    for (int i = 0; i < ModelShader::UniformsLength; i++) {
        uniforms[i] = glGetUniformLocation(program, uniformNames[i]);
    }
}

ModelShader* ModelShader::loadShader(const char* vertexSource,
                                     const char* fragmentSource) {
    ModelShader* shader = DBG_NEW ModelShader;
    
    shader->program = createProgram(vertexSource, fragmentSource);
    
    const char* uniformNames[ModelShader::UniformsLength] = {
        "texSampler", "modelMatrix", "rotMatrix", "alpha", "useAlpha",
        "waterHeight", "normalMapSampler", "shadowMap", "shadowViewMatrix",
        "shadowProjectionMatrix", "biasMatrix", "camPos"
    };
    
    for (int i = 0; i < ModelShader::UniformsLength; i++) {
        shader->uniforms[i] = glGetUniformLocation(shader->program, uniformNames[i]);
    }
    
    return shader;
}

int ModelShader::getUniformLocation(int uniform) {
    return this->uniforms[uniform];
}

int ModelShader::getShaderType() {
    return ShaderTypeModel;
}


void ShadowShader::LoadResource(const char *path) {
    Shader::LoadResource(path);
}

int ShadowShader::getUniformLocation(int uniform) {
    return this->uniforms[uniform];
}

int ShadowShader::getShaderType() {
    return ShaderTypeShadow;
}


void BlurShader::LoadResource(const char *path) {
    Shader::LoadResource(path);
    
    const char* uniformNames[BlurShader::UniformsLength] = {
        "blurDirection", "screenSize"
    };
    
    for (int i = 0; i < BlurShader::UniformsLength; i++) {
        uniforms[i] = glGetUniformLocation(program, uniformNames[i]);
    }
}

int BlurShader::getUniformLocation(int uniform) {
    return this->uniforms[uniform];
}

int BlurShader::getShaderType() {
    return ShaderTypeBlur;
}
