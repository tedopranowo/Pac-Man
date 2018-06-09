#pragma once

#include <lua.hpp>

namespace TransformComponentLua
{
	int SetPosition(lua_State* pState);
	int Move(lua_State* pState);
};