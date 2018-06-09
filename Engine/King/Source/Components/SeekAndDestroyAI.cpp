#include "SeekAndDestroyAI.h"

bool SeekAndDestroyAI::Init(const tinyxml2::XMLElement * pData)
{
    const tinyxml2::XMLElement* pElement    = pData->FirstChildElement("Speed");
    m_speed                                 = pElement->IntAttribute("spd");
    pElement                                = pData->FirstChildElement("Radius");
    m_radius                                = pElement->IntAttribute("rad");

    return true;
}