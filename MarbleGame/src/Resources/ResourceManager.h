//
//  ResourceManager.h
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__ResourceManager__
#define __MarbleGame__ResourceManager__

#include <string>
#include <map>

#include "../StringHash.h"

class IResource;
class Texture2DArray;
class Font;
class CEffectManager;

class ResourceManager {
public:
    template<class T>
    T* LoadContent(const char* path);
    void unloadAll();
	template<class T>
	void UnloadContent(T** resource);
    
    template<class T>
    T* QueueContent(const char* path);
	Texture2DArray* QueueTexture2DArray(const char* name, const char** paths, int numPaths);
	Font* QueueFont(const char* name, CEffectManager*);
    void LoadFromQueue();
    void TransferResourcesInQueue(ResourceManager* manager);
    
    static void SetResourcesPath(const std::string& path);
	static const std::string& GetResourcesPath() { return s_resourcesPath; }
private:
    typedef Hash ResourceKey;
    typedef std::pair<ResourceKey, IResource*> ResourcePair;
    typedef std::map<ResourceKey, IResource*> ResourceMap;
    
    typedef std::map<std::string, IResource*> LoadingQueueMap;
    typedef std::pair<std::string, IResource*> LoadingQueuePair;
    
    LoadingQueueMap m_queue;
    
    static std::string s_resourcesPath;
    ResourceMap resources;
    
    ResourceKey MakeResourceKey(const char* name);
    bool FileExists(const char* path);
};


/*
 * Implementation
 */

template <class T>
T* ResourceManager::LoadContent(const char *path) {
    ResourceKey key = MakeResourceKey(path);
    ResourceMap::iterator it = resources.find(key);
    
    if (it != resources.end()) {
        return static_cast<T*>(it->second);
    } else {
        T* res = DBG_NEW T;
		res->m_resourceKey = key;
		res->m_pResourceManager = this;
        res->LoadResource((s_resourcesPath + path).c_str());
        resources.insert(ResourcePair(key, res));
        return res;
    }
}

template <class T>
void ResourceManager::UnloadContent(T** resource) {
	T* temp = *resource;
	temp->Release();

	resources.erase(temp->m_resourceKey);
	delete temp;
	*resource = nullptr;
}

template <class T>
T* ResourceManager::QueueContent(const char *path) {
    ResourceKey key = MakeResourceKey(path);
    ResourceMap::iterator mIt = resources.find(key);
    
    if (mIt != resources.end()) {
        return static_cast<T*>(mIt->second);
    }
    
    LoadingQueueMap::iterator it = m_queue.find(path);
    
    if (it != m_queue.end()) {
        return static_cast<T*>(it->second);
    } else {
        T* res = DBG_NEW T;
		res->m_resourceKey = key;
		res->m_pResourceManager = this;
        m_queue.insert(LoadingQueuePair(path, res));
        return res;
    }
}

inline ResourceManager::ResourceKey ResourceManager::MakeResourceKey(const char* name) {
	return StringHash(name);
}

#endif /* defined(__MarbleGame__ResourceManager__) */
