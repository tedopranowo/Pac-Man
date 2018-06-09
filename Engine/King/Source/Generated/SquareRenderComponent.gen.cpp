#include "SquareRenderComponent.gen.h"
#include "../Components/SquareRenderComponent.h"
int SquareRenderComponentLua::SetSource(lua_State* pState)
{
	int x = static_cast<int>(lua_tointeger(pState, -2));
	int y = static_cast<int>(lua_tointeger(pState, -1));
	SquareRenderComponent* pSquareRenderComponent = static_cast<SquareRenderComponent*>(lua_touserdata(pState, -3));

	lua_pop(pState, 3);

	pSquareRenderComponent->SetSource(x, y);

	return 0;
}

void SquareRenderComponent::ExposeToLua(lua_State* pState)
{
	lua_createtable(pState, 0, 2);

	lua_pushlightuserdata(pState, this);
	lua_setfield(pState, -2, "this");

	lua_pushcfunction(pState, SquareRenderComponentLua::SetSource);
	lua_setfield(pState, -2, "SetSource");

	lua_setfield(pState, -2, "SquareRenderComponent");
}
