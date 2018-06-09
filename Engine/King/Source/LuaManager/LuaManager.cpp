#include "LuaManager.h"

#include "..\GameLogic\GameLogicBase.h"
#include "..\EventSystem\EventManager.h"
#include "..\ResourceManager\ResourceCache.h"
#include "..\ResourceManager\Resource.h"
#include "..\Actors\Actor.h"
#include "..\Events\ActorCreatedEvent.h"

using namespace std;

// TODO: Pass in Application and use accessors for logic and cache
LuaManager::LuaManager(GameLogicBase * pGameLogic, ResourceCache * pCache)
    : m_pGameLogic(pGameLogic)
    , m_pResourceCache(pCache)
    , m_isInitialized(false)
    , m_pLuaState(pGameLogic->GetLuaState())
	, m_customCFunctions()
{
    MAKE_AND_BIND_DELEGATE(m_actorCreatedDelegate, this, &LuaManager::BuildTableForActor, ActorCreatedEvent::s_kEventType);
}

LuaManager::~LuaManager()
{
    m_pGameLogic = nullptr;
    m_pResourceCache = nullptr;
    m_pLuaState = nullptr;
}

// Build a table for an actor in Lua
void LuaManager::BuildTableForActor(EventDataPtr eventData)
{
    shared_ptr<ActorCreatedEvent> pActorEvent = static_pointer_cast<ActorCreatedEvent>(eventData);
    pActorEvent->GetActor()->ExposeToLua(m_pGameLogic->GetLuaState());
}

// Load a lua file
void LuaManager::LoadFile(const std::string& filename)
{
    Resource toLoad(filename);
    std::shared_ptr<ResourceHandle> scriptHandle = m_pResourceCache->GetHandle(&toLoad);
}


//  This will expose the function defined in luaL_Reg to be callable by the lua script
void LuaManager::AddCustomCFunctions(std::vector<luaL_Reg>&& customFunctionDesc)
{
	m_customCFunctions = std::move(customFunctionDesc);
}

void LuaManager::ExposeCustomCFunctionsToLua()
{
	for (luaL_Reg& customFunctionDesc : m_customCFunctions)
	{
		//Push the function to the stack
		lua_pushcfunction(m_pLuaState, customFunctionDesc.func);

		//Pop the function with the given name attac_hed
		lua_setglobal(m_pLuaState, customFunctionDesc.name);
	}

	//Clear all the data inside m_customCFunctions
	m_customCFunctions.clear();
}