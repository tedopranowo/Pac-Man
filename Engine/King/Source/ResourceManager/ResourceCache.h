//------------------------- 
//Created by Shane King 
//Sat 04/01/2017 15:11:13.06 
//------------------------- 
#pragma once

#include <string>
#include <memory>
#include <list>
#include <map>
#include "../Actors/ActorFactory.h"

class ResourceHandle;
class IResourceLoader;
class IResourceFile;
class Resource;

typedef std::list<std::shared_ptr<ResourceHandle>> ResourceHandleList;
typedef std::map<std::string, std::shared_ptr<ResourceHandle>> ResourceHandleMap;
typedef std::list<std::shared_ptr<IResourceLoader>> ResourceLoaders;

class ResourceCache
{ 
protected: 
    ResourceHandleList m_leastRecentlyUsed;
    ResourceHandleMap m_resources;
    ResourceLoaders m_resourceLoaders;

    unsigned int m_cacheSize;
    unsigned int m_allocated;
    
    ActorFactory m_actorFactory;

public: 
	ResourceCache(const unsigned int sizeInMB);
	~ResourceCache();

    void Init();
    void LoadMultipleResourcesFromXml(const char* pathname);
    std::shared_ptr<ResourceHandle> GetResource(const char* name);
    std::shared_ptr<ResourceHandle> GetHandle(Resource * pResource);
private:
    bool WildcardMatch(const char* loaderPattern, const char* resourcePattern);
    //void RegisterLoader(std::shared_ptr<IResourceLoader> loader);
    std::shared_ptr<ResourceHandle> Find(Resource * pResource);
    std::shared_ptr<ResourceHandle> LoadSingleResource(std::string filename);
    std::string GetFilename(const char* path);
}; 
