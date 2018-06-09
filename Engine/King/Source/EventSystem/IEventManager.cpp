#include "IEventManager.h"
#include <cassert>

#include "../../../Logging/Source/Logger.h"

static IEventManager* g_pEventManager = nullptr;

/* 
When an Event Manager is created it will call this constructor (interface constructor)
This will check if g_pEventManager is already constructed, if so there's an error, we can only have 1 event manager,
if it's a nullptr it will set it to this Event Manager
*/ 
IEventManager::IEventManager(const char * pName, bool setAsGlobal)
{
    if (setAsGlobal)
    {
        if (g_pEventManager)
        {
            LOG_ERROR("Attempting to create a second global event manager.");
            delete g_pEventManager;
        }
        else
        {
            LOG_INIT(pName);
            g_pEventManager = this;
        }
    }
}

IEventManager::~IEventManager()
{
    if (g_pEventManager == this)
    {
        g_pEventManager = nullptr;
    }
}

IEventManager* IEventManager::Get()
{
    assert(g_pEventManager);
    return g_pEventManager;
}