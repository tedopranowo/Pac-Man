#include "ResourceHandle.h"

#include "ResourceCache.h"
#include "Resource.h"
#include "GraphicResource.h"

//--------------------------------------------------------------- 
// Public Methods 
//---------------------------------------------------------------- 

ResourceHandle::ResourceHandle(std::shared_ptr<Resource> resource)
{
    m_pResource = resource;
}

ResourceHandle::~ResourceHandle()
{
   // m_pResourceCache->MemoryHasBeenFreed(m_size);
    m_pResource.reset();
}
