//
//  MaterialList.h
//  MarbleGame
//
//  Created by Tarik Karaca on 01.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__MaterialList__
#define __MarbleGame__MaterialList__

#include <string>
#include <map>

#include "Resource.h"
#include "Scene/Material.h"

struct ResMaterial {
    std::string m_name;
    std::string m_diffuseMap;
    std::string m_normalMap;
};

class MaterialList : public IResource {
public:
    typedef size_t Hash;
    typedef std::map<Hash, ResMaterial> MaterialMap;
    typedef std::pair<Hash, ResMaterial> MaterialPair;
    
    MaterialMap m_materials;
public:
    void LoadResource(const char* path);
    void Release();
    
    Material GetVisualMaterial(const char* name);
};

#endif /* defined(__MarbleGame__MaterialList__) */
