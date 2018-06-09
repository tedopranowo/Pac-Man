#pragma once
#include "AIComponent.h"

class SeekAndDestroyAI : public AIComponent
{
private:
    int m_speed;
    int m_radius;

public:
    SeekAndDestroyAI() {}
    ~SeekAndDestroyAI() {}
    bool Init(const tinyxml2::XMLElement * pData);
};

