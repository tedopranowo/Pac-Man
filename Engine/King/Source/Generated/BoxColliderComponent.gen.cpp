#include "BoxColliderComponent.gen.h"
#include "../Components/BoxColliderComponent.h"
int BoxColliderComponentLua::SetPosition(lua_State* pState)
{
	int x = static_cast<int>(lua_tointeger(pState, -2));
	int y = static_cast<int>(lua_tointeger(pState, -1));
	BoxColliderComponent* pBoxColliderComponent = static_cast<BoxColliderComponent*>(lua_touserdata(pState, -3));

	lua_pop(pState, 3);

	pBoxColliderComponent->SetPosition(x, y);

	return 0;
}

void BoxColliderComponent::ExposeToLua(lua_State* pState)
{
	lua_createtable(pState, 0, 2);

	lua_pushlightuserdata(pState, this);
	lua_setfield(pState, -2, "this");

	lua_pushcfunction(pState, BoxColliderComponentLua::SetPosition);
	lua_setfield(pState, -2, "SetPosition");

	lua_setfield(pState, -2, "BoxColliderComponent");
}
