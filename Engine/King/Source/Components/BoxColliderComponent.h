#pragma once

#include "ActorComponent.h"


#include <SDL.h>
#include <vector>


#include "..\LuaManager\TypedefLua.h"
#include "..\Generated\BoxColliderComponent.gen.h"

class BoxColliderComponent;

typedef void (*Reaction)();

class BoxColliderComponent : public ActorComponent
{
private:
    constexpr static ComponentId m_kId{ ActorComponent::BoxColliderComponentID };
	constexpr static char* m_kWallId{ "Wall" };
	static int s_newReactionId;
    std::string m_reactId;
    
	SDL_Rect m_collider;

    bool m_isTrigger;
    
	std::map<std::string, const char*> m_luaReactions;

public:
    BoxColliderComponent();
    ~BoxColliderComponent() {}

	// ActorComponent overrides
    bool Init(const tinyxml2::XMLElement* pData) override;
    virtual void Update(double deltaSeconds) override;
    void Draw(SDL_Renderer* pRen, const Vec2& cameraPosition);
    virtual StrongActorComponentPtr Clone() override;
    virtual const ComponentId GetComponentId() const override { return m_kId; }
    
	bool IsTrigger() { return m_isTrigger; }

    virtual SDL_Rect* GetRect() { return &m_collider; }
    const std::string GetReactId() const { return m_reactId; }

	void AddLuaReaction(std::string id, const char* func) { m_luaReactions.insert(std::make_pair(id, func)); }

	void CallLuaReaction(lua_State* pState, std::string id, LuaRef otherRef);

	void CallLuaReactionWall(lua_State * pState);

    // Lua Functions
    void ExposeToLua(lua_State* pState);

    LuaFunction()
    void SetPosition(int x, int y) { m_collider.x = x; m_collider.y = y; }
};

