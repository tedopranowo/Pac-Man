#pragma once

#include "TypedefActors.h"
#include <lua.hpp>

class Actor
{
private:
	ActorId m_Id;
	LuaRef m_luaRef{ 0 };
    ActorComponents m_components;

public:
    Actor() {}
    ~Actor() {}

    void Destroy();
    StrongActorPtr Clone();
    void Update(double deltatime);

    // mutators
    void AddComponent(StrongActorPtr actor, StrongActorComponentPtr pComponent);
    void SetActorId(ActorId id) { m_Id = id; }
	void SetLuaRef(LuaRef ref) { m_luaRef = ref; }

    // accessors
    bool HasComponent(const ComponentId componentId) const;
    StrongActorComponentPtr GetComponent(const ComponentId componentId) const;
    const ActorId GetActorId() const { return m_Id; }
	const LuaRef GetLuaRef() const { return m_luaRef; }
    
    // Lua Functions
    void ExposeToLua(lua_State* pState);
};