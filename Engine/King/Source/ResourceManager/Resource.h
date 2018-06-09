//------------------------- 
//Created by Shane King 
//Sat 04/01/2017 13:34:07.72 
//------------------------- 
#pragma once

#include <string>

struct SDL_Surface;

class Resource 
{  
private:
	/* Private Variables */
    std::string m_name;

public:
	/* Public Methods */
    Resource(const std::string& name);
	~Resource() {}

    const std::string GetName() { return m_name; }
};