#pragma once

#include "ActorComponent.h"
#include <SDL.h>

#include "..\LuaManager\TypedefLua.h"
#include "..\Generated\SquareRenderComponent.gen.h"

class Vec2;

class SquareRenderComponent : public ActorComponent
{
private:
    constexpr static ComponentId m_kId{ ActorComponent::SquareRenderComponentID };
    
protected:
    SDL_Surface* m_pSurface;
    SDL_Texture* m_pTexture;
    SDL_Rect m_src;
    SDL_Rect m_dst;

public:
    SquareRenderComponent();
    SquareRenderComponent(double x, double y, double w, double h, SDL_Surface* filename);
    virtual ~SquareRenderComponent() {}

	// ActorComponent overrides
    virtual bool Init(const tinyxml2::XMLElement * pData) override;
    virtual void Update(double deltaTime) override;
    virtual void Draw(SDL_Renderer* pRen, const Vec2& cameraPos) override;
    virtual StrongActorComponentPtr Clone() override;
    virtual const ComponentId GetComponentId() const override { return m_kId; };

	// Lua Functions
    void ExposeToLua(lua_State* pState);

    LuaFunction()
    void SetSource(int x, int y) { m_src.x = x; m_src.y = y; }
};