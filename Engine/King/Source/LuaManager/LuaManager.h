#pragma once

#include <string>
#include <vector>
#include "..\Application\Application.h"
#include <lua.hpp>


#include "..\EventSystem\TypedefEvents.h"

class GameLogicBase;
class ResourceCache;

class LuaManager
{
private:
    GameLogicBase* m_pGameLogic;
    ResourceCache* m_pResourceCache;
    lua_State* m_pLuaState;
    bool m_isInitialized;

	std::vector<luaL_Reg> m_customCFunctions;

    EventListenerDelegate m_actorCreatedDelegate;

public:
    LuaManager(GameLogicBase* pGameLogic, ResourceCache* pCache);
    ~LuaManager();

    void LoadFile(const std::string& filename);

	//Custom C Functions
	void AddCustomCFunctions(std::vector<luaL_Reg>&& customFunctionDesc);
	void ExposeCustomCFunctionsToLua();

private:
    void BuildTableForActor(EventDataPtr eventData);
};