//
//  ResourceManager.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "ResourceManager.h"
#include "Resource.h"

#include "UserInterface/Font.h"

std::string ResourceManager::s_resourcesPath = "";

void ResourceManager::unloadAll() {
    ResourceMap::iterator it;
    for (it = resources.begin(); it != resources.end(); it++) {
        it->second->Release();
        delete it->second;
    }
    
    resources.clear();
}

Texture2DArray* ResourceManager::QueueTexture2DArray(const char* name, const char** paths, int numPaths) {
	ResourceKey key = MakeResourceKey(name);
	ResourceMap::iterator mIt = resources.find(key);

	if (mIt != resources.end()) {
		return static_cast<Texture2DArray*>(mIt->second);
	}

	LoadingQueueMap::iterator it = m_queue.find(name);

	if (it != m_queue.end()) {
		return static_cast<Texture2DArray*>(it->second);
	}
	else {
		Texture2DArray* res = DBG_NEW Texture2DArray();
		res->m_resourceKey = key;
		res->m_pResourceManager = this;
		res->SetTextureNames(paths, numPaths);
		m_queue.insert(LoadingQueuePair(name, res));
		return res;
	}
}

Font* ResourceManager::QueueFont(const char* name, CEffectManager* pManager) {
	ResourceKey key = MakeResourceKey(name);
	ResourceMap::iterator mIt = resources.find(key);

	if (mIt != resources.end()) {
		return static_cast<Font*>(mIt->second);
	}

	LoadingQueueMap::iterator it = m_queue.find(name);

	if (it != m_queue.end()) {
		return static_cast<Font*>(it->second);
	}
	else {
		Font* res = DBG_NEW Font(pManager);
		res->m_resourceKey = key;
		m_queue.insert(LoadingQueuePair(name, res));
		return res;
	}
}

void ResourceManager::LoadFromQueue() {
    LoadingQueueMap::iterator it;
    
    for (it = m_queue.begin(); it != m_queue.end(); ++it) {
        const std::string& path = it->first;
        IResource* resource = it->second;
        ResourceKey key = MakeResourceKey(path.c_str());
        
        std::string fullPath = s_resourcesPath + path;
        /*if (!FileExists(fullPath.c_str())) {
			char errorString[128];
			sprintf_s(errorString, "File does not exist: %s\n", path.c_str());
			OutputDebugStringA(errorString);
            fprintf(stderr, "%s: File does not exist.\n", fullPath.c_str());
            continue;
        }*/
        
        resource->LoadResource(fullPath.c_str());
        resources.insert(ResourcePair(key, resource));
    }
    
    LoadingQueueMap().swap(m_queue);
}

void ResourceManager::TransferResourcesInQueue(ResourceManager *manager) {
    if (!manager)
        return;
    
    LoadingQueueMap::iterator it;
    
    for (it = m_queue.begin(); it != m_queue.end();) {
        const std::string& path = it->first;
        IResource* resource = it->second;
        
        ResourceKey key = MakeResourceKey(path.c_str());
        ResourceMap::iterator resIt = manager->resources.find(key);
        
        if (resIt != manager->resources.end()) {
            resIt->second->Copy(resource);
            delete resIt->second;
            manager->resources.erase(resIt);
            it = m_queue.erase(it);
        }
    }
}

bool ResourceManager::FileExists(const char *path) {
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    } else
        return false;
}

void ResourceManager::SetResourcesPath(const std::string& path) {

	s_resourcesPath.reserve(128);
	char slash = '/';
	
	size_t match = path.find_first_of(slash);
	if (match == std::string::npos) {
		slash = '\\';
	}

	s_resourcesPath = path;
	if (s_resourcesPath.back() != slash) {
		s_resourcesPath += slash;
	}
}
