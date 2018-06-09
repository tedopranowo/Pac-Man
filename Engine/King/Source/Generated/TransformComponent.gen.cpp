#include "TransformComponent.gen.h"
#include "../Components/TransformComponent.h"
int TransformComponentLua::SetPosition(lua_State* pState)
{
	double x = static_cast<double>(lua_tonumber(pState, -2));
	double y = static_cast<double>(lua_tonumber(pState, -1));
	TransformComponent* pTransformComponent = static_cast<TransformComponent*>(lua_touserdata(pState, -3));

	lua_pop(pState, 3);

	pTransformComponent->SetPosition(x, y);

	return 0;
}

int TransformComponentLua::Move(lua_State* pState)
{
	double x = static_cast<double>(lua_tonumber(pState, -2));
	double y = static_cast<double>(lua_tonumber(pState, -1));
	TransformComponent* pTransformComponent = static_cast<TransformComponent*>(lua_touserdata(pState, -3));

	lua_pop(pState, 3);

	pTransformComponent->Move(x, y);

	return 0;
}

void TransformComponent::ExposeToLua(lua_State* pState)
{
	lua_createtable(pState, 0, 3);

	lua_pushlightuserdata(pState, this);
	lua_setfield(pState, -2, "this");

	lua_pushcfunction(pState, TransformComponentLua::SetPosition);
	lua_setfield(pState, -2, "SetPosition");

	lua_pushcfunction(pState, TransformComponentLua::Move);
	lua_setfield(pState, -2, "Move");

	lua_setfield(pState, -2, "TransformComponent");
}
