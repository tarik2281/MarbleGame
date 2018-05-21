//
//  MaterialList.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 01.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//


#include "pugixml.hpp"

#include "MaterialList.h"
#include "ResourceManager.h"
#include "StringHash.h"
#include "main.h"

#ifndef _WINDLL
#define Load QueueContent
#else
#define Load LoadContent
#endif

void MaterialList::LoadResource(const char *path) {
    pugi::xml_document doc;
    doc.load_file(path);
    pugi::xml_node materialsNode = doc.child("Materials");
    pugi::xml_node_iterator it;
    for (it = materialsNode.begin(); it != materialsNode.end(); ++it) {
        if (strcmp(it->name(), "Material") == 0) {
            const char* name = it->attribute("name").as_string();
            Hash hash = StringHash(name);
            ResMaterial mat;
            mat.m_name = name;
            mat.m_diffuseMap = it->child("DiffuseMap").text().as_string();
            mat.m_normalMap = it->child("NormalMap").text().as_string();
            m_materials.insert(MaterialPair(hash, mat));
        }
    }
}

void MaterialList::Release() {

}

Material MaterialList::GetVisualMaterial(const char *name) {
    Hash hash = StringHash(name);
    ResMaterial resMat = m_materials.find(hash)->second;
    Material mat;
	mat.identifier = hash;
	mat.opacity = 1.0f;
    mat.diffuseMap = g_pResourceManager->Load<Texture2D>(resMat.m_diffuseMap.c_str());
	mat.normalMap = g_pResourceManager->Load<Texture2D>(resMat.m_normalMap.c_str());
    return mat;
}
