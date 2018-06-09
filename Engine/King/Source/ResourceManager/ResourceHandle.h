#pragma once
//------------------------- 
//Created by Shane King 
//Sat 04/01/2017 14:26:24.10 
//------------------------- 

#include <memory>

class Resource;
class ResourceCache;

class ResourceHandle 
{ 
private:
	/* Public Variables */
    std::shared_ptr<Resource> m_pResource;

public: 
	/* Public Methods */ 
	ResourceHandle(std::shared_ptr<Resource> resource);
	virtual ~ResourceHandle();

    std::shared_ptr<Resource> GetResource() { return m_pResource; }
}; 
