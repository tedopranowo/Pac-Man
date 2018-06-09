#include <Application\Application.h>
#include <Physics\Vec2.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "PacmanGame.h"

PacmanGame g_pacmanGame = PacmanGame();

int L_RegisterUnit(lua_State* L)
{
	int id = static_cast<int>(lua_tonumber(L, 1));
	Unit::Type type = static_cast<Unit::Type>(static_cast<int>(lua_tonumber(L, 2)));
	int x = static_cast<int>(lua_tonumber(L, 3));
	int y = static_cast<int>(lua_tonumber(L, 4));

	g_pacmanGame.RegisterUnit(id, type, Vec2(x, y));

	return 0;
}

int L_GetAiMove(lua_State* L)
{
	int id = static_cast<int>(lua_tonumber(L, 1));

	Vec2 movementDirection = g_pacmanGame.GetUnitWithId(id)->GetAi()->GetMove();

	lua_pushnumber(L, movementDirection.GetX());
	lua_pushnumber(L, movementDirection.GetY());

	return 2;
}

int L_ReadCollisionMap(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	g_pacmanGame.LoadMap(filePath);

	return 0;
}

int L_IsWalkable(lua_State* L)
{
	size_t x = static_cast<size_t>(lua_tonumber(L, 1));
	size_t y = static_cast<size_t>(lua_tonumber(L, 2));

	bool isWalkable = g_pacmanGame.GetMap().IsWalkable(x, y);
	
	lua_pushboolean(L, isWalkable);

	return 1;
}

int L_IsIntersection(lua_State* L)
{
	size_t x = static_cast<size_t>(lua_tonumber(L, 1));
	size_t y = static_cast<size_t>(lua_tonumber(L, 2));

	bool isIntersection = g_pacmanGame.GetMap().IsIntersection(x, y);

	lua_pushboolean(L, isIntersection);

	return 1;
}

int L_UpdatePosition(lua_State* L)
{
	int id = static_cast<size_t>(lua_tonumber(L, 1));
	int x = static_cast<int>(lua_tonumber(L, 2));
	int y = static_cast<int>(lua_tonumber(L, 3));

	Unit* pUnit = g_pacmanGame.GetUnitWithId(id);
	pUnit->SetPosition(Vec2(x, y));

	return 0;
}

int L_UpdateDirection(lua_State* L)
{
	int id = static_cast<size_t>(lua_tonumber(L, 1));
	int x = static_cast<int>(lua_tonumber(L, 2));
	int y = static_cast<int>(lua_tonumber(L, 3));

	g_pacmanGame.GetUnitWithId(id)->SetDirection(Vec2(x, y));

	return 0;
}

int L_Update(lua_State* L)
{
	int id = static_cast<size_t>(lua_tonumber(L, 1));
	double deltaTime = lua_tonumber(L, 2);

	g_pacmanGame.GetUnitWithId(id)->Update(deltaTime);

	return 0;
}

//Reset ghost state (from scared)
int L_SetGhostFrightenedStatus(lua_State* L)
{
	int id = static_cast<size_t>(lua_tonumber(L, 1));
	bool isFrightened = lua_toboolean(L, 2);
	
	g_pacmanGame.GetUnitWithId(id)->GetAi()->SetFrightened(isFrightened);

	return 0;
}

//Find path home
int L_FindPathHome(lua_State* L)
{
	int id = static_cast<int>(lua_tonumber(L, 1));

	Vec2 movementDirection = g_pacmanGame.GetUnitWithId(id)->GetAi()->FindPathHome();

	lua_pushnumber(L, movementDirection.GetX());
	lua_pushnumber(L, movementDirection.GetY());

	return 2;
}

int L_GetGhostHomePosition(lua_State* L)
{
	Vec2 movementDirection = g_pacmanGame.GetMap().GetHomePosition();

	lua_pushnumber(L, movementDirection.GetX());
	lua_pushnumber(L, movementDirection.GetY());

	return 2;
}

int main(int argc, char* argv[])
{
    Application app;
	Application* pApp = &app;

	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, getAIMoveDescriptor, "GetAiMove", L_GetAiMove);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, readCollisionMapDescriptor, "ReadCollisionMap", L_ReadCollisionMap);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, isIntersectionDescriptor, "IsIntersection", L_IsIntersection);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, isWalkableDescriptor, "IsWalkable", L_IsWalkable);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, registerUnitDescriptor, "RegisterUnit", L_RegisterUnit);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, updatePositionDescriptor, "UpdatePosition", L_UpdatePosition);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, updateDirectionDescriptor, "UpdateDirection", L_UpdateDirection);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, updateDescriptor, "Update", L_Update);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, setGhostFrightenedStatusDescriptor, "SetGhostFrightenedStatus", L_SetGhostFrightenedStatus);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, FindPathHomeDescriptor, "FindPathHome", L_FindPathHome);
	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, GetGhostHomePositionDescriptor, "GetGhostHomePosition", L_GetGhostHomePosition);
	
	//Run the application
    app.Run();
    app.Cleanup();

    return 0;
}