#include "ResourceCache.h" 

#include "DefaultResourceLoader.h"
#include "Resource.h"
#include "GraphicResource.h"
#include "ResourceHandle.h"
#include "GraphicResourceLoader.h"
#include "ActorResourceLoader.h"
#include "../Actors/ActorFactory.h"
#include "..\Xml\tinyxml2.h"
#include "..\..\..\Logging\Source\Logger.h"

//--------------------------------------------------------------- 
// Public Methods 
//---------------------------------------------------------------- 

ResourceCache::ResourceCache(const unsigned int sizeInMB)
    : m_actorFactory(ActorFactory())
    , m_cacheSize(0)
    , m_allocated(0)
{
    m_cacheSize = sizeInMB * 1024 * 1024;
}

ResourceCache::~ResourceCache()
{
    m_resources.clear();
}

/*
Register resource loaders
*/
void ResourceCache::Init()
{
    m_resourceLoaders.push_back(std::shared_ptr<ActorResourceLoader>(std::make_shared<ActorResourceLoader>(&m_actorFactory)));
    m_resourceLoaders.push_back(std::shared_ptr<GraphicResourceLoader>(std::make_shared<GraphicResourceLoader>()));
}

/*
This function takes the name of the directory that holds all the files
to be loaded.  The directory has to have an .xml that contains the list
of all the files to be uploaded.  The resource cache will handle the rest.
*/
void ResourceCache::LoadMultipleResourcesFromXml(const char* pathname)
{
    tinyxml2::XMLDocument assetList;
    if (assetList.LoadFile(pathname) != 0)
    {
        LOG_WARNING("Unable to load file");
        assetList.PrintError();
        return;
    }

    // Pointer to resource loader
    IResourceLoader* loader{ nullptr };

    // Loop through each resource type, and load the resources of that type
    for (const tinyxml2::XMLElement* category = assetList.RootElement()->FirstChildElement();
    category != 0; category = category->NextSiblingElement())
    {
        // Type of resources we will be loading
        const char* fileType = category->Name();
        
        // Find the correct resource loader
        for (auto loaderIt : m_resourceLoaders)
        {  
            bool test = WildcardMatch(loaderIt->GetPattern().c_str(), fileType);
            if (test)
            {
                loader = loaderIt.get();
                break;
            }
        }

        // Unable to find a matching resource loader
        if (loader == nullptr)
        {
            LOG_WARNING("Unable to find a resource loader");
            continue;
        }

        // Load each resource of this resource type with the resource loader into the resource cache
        for (const tinyxml2::XMLElement* asset = category->FirstChildElement();
            asset != 0; asset = asset->NextSiblingElement())
        {
            std::string assetName = asset->GetText();
            size_t startPos = assetName.find_last_of("\\");
            std::string documentName = assetName.substr(startPos + 1);

			// Load the resource
            std::shared_ptr<ResourceHandle> handle = loader->LoadResource(assetName.c_str());

            if (handle)
            {
                // found the resource, cache it
                m_resources.insert(std::make_pair(documentName, handle));
            }
            else
            {
                // Unable to find the resource file (this would be the actual xml, png, or any other resource file)
                LOG_ERROR("Resource Cache Error: Unable to find resource file to load.");
            }
        }
    }
}

/*
Grabs the resource handle from the cache
*/
std::shared_ptr<ResourceHandle> ResourceCache::GetResource(const char* name)
{
    std::string resource = GetFilename(name);
    std::shared_ptr<ResourceHandle> handle = m_resources[resource];

    // I should log error here instead, but I need to setup logging so it can take %s %d values.
    assert(handle);
    return handle;
}

/*
This compares the type of resource based on the xml (i.e. ActorData, PngFiles) and 
compares it with the type of loader
*/
bool ResourceCache::WildcardMatch(const char * loaderPattern, const char * resourceFilename)
{
    return *loaderPattern == *resourceFilename;
}

/*
Check if resource is loaded in the cache.
If so, update it.
If not, cache miss, and load the resource.
*/
std::shared_ptr<ResourceHandle> ResourceCache::GetHandle(Resource* pResource)
{
    std::shared_ptr<ResourceHandle> handle(Find(pResource));
    if (handle == nullptr) // Cache miss
        handle = LoadSingleResource(pResource->GetName());
    return handle;
}

std::shared_ptr<ResourceHandle> ResourceCache::Find(Resource * pResource)
{
    std::shared_ptr<ResourceHandle> handle = m_resources[pResource->GetName()];
    return handle;
}

std::shared_ptr<ResourceHandle> ResourceCache::LoadSingleResource(std::string filename)
{
    // Pointer to resource loader
    IResourceLoader* loader{ nullptr };

    // Find the correct resource loader
    for (auto loaderIt : m_resourceLoaders)
    {
        bool test = WildcardMatch(loaderIt->GetPattern().c_str(), filename.c_str());
        if (test)
        {
            loader = loaderIt.get();
            break;
        }
    }

    // Unable to find a matching resource loader
    if (loader == nullptr)
    {
        LOG_WARNING("Unable to find a resource loader");
        return nullptr;
    }

    // Load the resource with the correct resource loader
    size_t startPos = filename.find_last_of("\\");
    std::string documentName = filename.substr(startPos + 1);
    std::shared_ptr<ResourceHandle> handle = loader->LoadResource(filename.c_str());
    m_resources.insert(std::make_pair(documentName, handle));
    return handle;
}

// Get a filename from a path name (i.e. Assets\\Actors\\MyActor.xml to MyActor.xml)
std::string ResourceCache::GetFilename(const char* path)
{
    std::string pathname = path;
    size_t startPos = pathname.find_last_of("\\");
    std::string filename = pathname.substr(startPos + 1).c_str();
    return filename;
}