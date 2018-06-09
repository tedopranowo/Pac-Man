#pragma once

#include "../Xml/tinyxml2.h"
#include "../Physics/Vec2.h"
#include "../Actors/Actor.h"
#include <lua.hpp>
#include <SDL.h>

//--------------------------------------------------------------------------------------
//  This is the base class for all components in the engine.  Components are the building
//  blocks for Actors.  The components provide all functionallity to an Actor.  Every
//  component needs a unique ID.  There are a lot of virtual functions for this class.
//      -m_pOwner: this is a shared_ptr to the actor containing this component
//--------------------------------------------------------------------------------------
class ActorComponent
{
public:
    enum Id
    {
        SquareRenderComponentID     = 0x55558204,
        TransformComponentID        = 0x43e5ca46,
        BoxColliderComponentID      = 0x3cea2a75,

        None = 0
    };

private:
    friend class ActorFactory;

protected:
    StrongActorPtr m_pOwner;

public:
    ActorComponent();
    virtual ~ActorComponent() {}

    // methods
	virtual bool Init(const tinyxml2::XMLElement* pData) = 0;
	virtual void Update(double deltaSeconds) = 0;
	virtual void Draw(SDL_Renderer* pRen, const Vec2& camera) { pRen; camera; }
    virtual StrongActorComponentPtr Clone() = 0;
    virtual const ComponentId GetComponentId() const = 0;
	

	// owner methods
    StrongActorPtr GetOwner() { return m_pOwner; }
    void SetOwner(StrongActorPtr pOwner) { m_pOwner = pOwner; }
    void RemoveOwner();

    // lua
	virtual void ExposeToLua(lua_State* pState) = 0;
};

