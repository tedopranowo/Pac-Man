#include "BoxColliderComponent.h"

#include "TransformComponent.h"
#include "SquareRenderComponent.h"
#include <memory>
#include <cassert>
#include <iterator>

#pragma warning(push)
#pragma warning(disable:4100)

//--------------------------------------------------------------------------------------
//  This component serves as the collider for actors, this is used for collision detection
//  with other actors.
//		m_kWallId:		This is the static reaction id for all boxcollider components.
//						Add a Reaction with that key "Wall", to make that Reaction the
//						callback whenever this actor hits a wall.
//      m_collider:     This is an SDL_Rect, which is a struct with x, y, w, h. This is
//                      the actual dimensions and position of the collider
//      m_isTrigger:    This will make this collider a trigger, so no physical collision
//                      will occur.
//      m_reactId:      This ID is used by other actors that are colliding with this
//                      collider to decipher the type of actor it's colliding with. This
//                      allows actors to react differently depending on the object it's
//                      colliding with.
//      m_reactions:    This is a map of funciton pointers
//                          -key: the ID (m_reactId) of the type of actor it's colliding with
//                          -value: the callback function to run when the collision occurs
//--------------------------------------------------------------------------------------
using namespace std;

BoxColliderComponent::BoxColliderComponent()
	: m_isTrigger(false)
{
}

//--------------------------------------------------------------------------------------
//  Initialize this box collider with the data from the xml
//--------------------------------------------------------------------------------------
bool BoxColliderComponent::Init(const tinyxml2::XMLElement* pData)
{
    const tinyxml2::XMLElement* xElement = pData->FirstChildElement("Dimensions");
    
    m_collider.x = xElement->IntAttribute("x");
    m_collider.y = xElement->IntAttribute("y");
    m_collider.w = xElement->IntAttribute("width");
    m_collider.h = xElement->IntAttribute("height");

	xElement = pData->FirstChildElement("Reaction");
		if (xElement != 0)
			m_reactId = xElement->Attribute("reactionid");

    xElement = pData->FirstChildElement("Trigger");

    // If there's no attribute trigger than use the default value of false
	if(xElement)
		if (xElement->FindAttribute("trigger") != 0)
			m_isTrigger = xElement->BoolAttribute("trigger");
    
	return true;
}

//--------------------------------------------------------------------------------------
//  Update the box collider. Check if our owner has a transform component, if so the box 
//  collider should set to the transform's location.  Otherwise, this boxcollider is 
//  static and won't move.
//--------------------------------------------------------------------------------------
void BoxColliderComponent::Update(double deltaSeconds)
{
    shared_ptr<TransformComponent> pTransform = 
        static_pointer_cast<TransformComponent>(m_pOwner->GetComponent(ActorComponent::TransformComponentID));

    // Owner has a transform
    if (pTransform)
    {
        // Set the position of the collider to the transform's position
        Vec2 posVec = pTransform->GetPosition();
        SetPosition((int)posVec.GetX(), (int)posVec.GetY());
    }
}

//--------------------------------------------------------------------------------------
//  Create a clone of this component, this is called by it's owner (Actor) when creating
//  a clone of the owner (Actor).
//      -return: a shared_ptr to the clone of this component
//--------------------------------------------------------------------------------------
StrongActorComponentPtr BoxColliderComponent::Clone()
{
    BoxColliderComponent clone = *this;
    clone.m_collider = this->m_collider;
    clone.m_isTrigger = this->m_isTrigger;

    return StrongActorComponentPtr(std::make_shared<BoxColliderComponent>(clone));
}

//--------------------------------------------------------------------------------------
//  This will draw the collision rect, this is used when debugging, should not be used
//  in the actual game for shipping.
//--------------------------------------------------------------------------------------
void BoxColliderComponent::Draw(SDL_Renderer* pRen, const Vec2& cameraPosition)
{
    //// Get the destination to draw the rect in screen space
    SDL_Rect dst;

    dst.x = m_collider.x + (int)cameraPosition.GetX();
    dst.y = m_collider.y + (int)cameraPosition.GetY();
    dst.w = m_collider.w;
    dst.h = m_collider.h;

    // Draw the rect green
    SDL_SetRenderDrawColor(pRen, 0, 255, 0, 255);
    SDL_RenderDrawRect(pRen, &dst);
    SDL_SetRenderDrawColor(pRen, 0, 0, 0, 255);
}

//-------------------------------------------------------------------------------------------
// This function is called whenever the actor with this collider component collides with
// another actor.  This will check if this function will call the appropriate Reaction function
// depending on what it's colliding with.
//      pState:   	The lua state
//		id:			The id of the other actor the owner of this box collider is colliding with
//		otherRef:	The reference to the Lua Table of the other actor.
//-------------------------------------------------------------------------------------------
void BoxColliderComponent::CallLuaReaction(lua_State *pState, std::string id, LuaRef otherRef)
{
	// Get this actor's reference to it's Lua Table
	LuaRef actorRef = this->GetOwner()->GetLuaRef();
	assert(actorRef > 0);
	assert(otherRef > 0);
	
	// Get the appropriate reaction function
	std::map<std::string, const char*>::iterator it = m_luaReactions.find(id);
	if (it != m_luaReactions.end())
	{
		const char* func = it->second;
		lua_getglobal(pState, func);
		lua_rawgeti(pState, LUA_REGISTRYINDEX, actorRef);
		lua_rawgeti(pState, LUA_REGISTRYINDEX, otherRef);

		// Call the Reaction function
		lua_call(pState, 2, 0);
	}
}

//-------------------------------------------------------------------------------------------
// This function is called whenever the actor with this collider component collides with
// a wall.  The constant id for reactions to walls is "Wall".
//      pState:   	The lua state
//-------------------------------------------------------------------------------------------
void BoxColliderComponent::CallLuaReactionWall(lua_State *pState)
{
	LuaRef actorRef = this->GetOwner()->GetLuaRef();
	assert(actorRef > 0);

	std::map<std::string, const char*>::iterator it = m_luaReactions.find(m_kWallId);
	if (it != m_luaReactions.end())
	{
		const char* func = it->second;
		lua_getglobal(pState, func);
		lua_rawgeti(pState, LUA_REGISTRYINDEX, actorRef);

		lua_call(pState, 1, 0);
	}
}
#pragma warning(pop)
