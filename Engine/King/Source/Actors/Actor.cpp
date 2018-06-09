// Actor.cpp

#include "Actor.h"

#include "../Components/SquareRenderComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventSystem/EventManager.h"
#include "../Physics/Vec2.h"
#include "../LuaManager/TypedefLua.h"
#include "../../../Logging/Source/Logger.h"

//---------------------------------------------------------------------------
//  The Actor class is essentially a group of components.  By itself the class
//  should not have any functionallity.  When updating an actor, we simply
//  loop though all of it's components and call their individual Update().
//
//      -m_Id:  this is a unique ID for this actor.  No 2 actors should have
//              the same ID.
//      -m_components:  this is a map of all of this actor's components.  An
//                      actor is not allowed to have 2 of the same component
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  This should be called when destroying an actor.  This will release the
//  resources held by the component shared_ptrs.  This will also release the
//  resource (this actor) held by the component's shared_ptr to it's owner.
//---------------------------------------------------------------------------
void Actor::Destroy()
{
    // loop through the components and release the resources
    for (auto component : m_components)
    {
        component.second->RemoveOwner();
        component.second.reset();
    }
}

//---------------------------------------------------------------------------
//  This will do a deep copy of the actor class.
//  I'm delibaretly choosing not to use universal reference,
//  I want a copy of the actor and a copy of the components.
//      -return: a clone of this actor  
//---------------------------------------------------------------------------
StrongActorPtr Actor::Clone()
{
    // create a new Actor with no components
    StrongActorPtr clone = std::make_shared<Actor>();

    // loop through this actors components
    for (auto component : this->m_components)
    {
        // create a clone of the component
        StrongActorComponentPtr componentClone = component.second->Clone();

        // add the component clone to the new actor
        clone->AddComponent(clone, componentClone);
    }
    return clone;
}

//---------------------------------------------------------------------------
//  This will update all of the actor's components.
//---------------------------------------------------------------------------
void Actor::Update(double deltatime)
{
    // loop through the components and call it's update function
    for (auto component : m_components)
    {
        component.second.get()->Update(deltatime);
    }
}

//---------------------------------------------------------------------------
//  This function will attempt to add a component to the actor's map of components
// 
//  NOTE: I had to pass in StrongActorPtr due to the way that smart pointers work.
//
//  When calling SetOwner(), do not do SetOwner(this), this will create 2
//  different "object managers" (used by smart pointers), this will mess up the 
//  strong ref count.  We only want 1 object manager so the ref count is accurate.
//---------------------------------------------------------------------------
void Actor::AddComponent(StrongActorPtr actor, StrongActorComponentPtr pComponent)
{
    // check if this actor already has a component of this type
    auto componentIterator = m_components.find(pComponent->GetComponentId());

    // if the actor does NOT have this component
    if (componentIterator == m_components.end())
    {
        // add the component
        pComponent->SetOwner(actor);
        m_components.insert(std::make_pair(pComponent->GetComponentId(), pComponent));
        return;
    }

    // this actor already has this component
    LOG_ERROR("Attempting to add component twice.");
}

//---------------------------------------------------------------------------
//  This function will check if the actor has a component
//      -componentId: the id of the component we want to check
//      -return:    True, if the actor has the component.  False, otherwise
//---------------------------------------------------------------------------
bool Actor::HasComponent(const ComponentId componentId) const
{
    auto componentIt = m_components.find(componentId);
    return (componentIt != m_components.end());
}

//---------------------------------------------------------------------------
//  This function will grab and return a desired component
//      -componentId: the id of the component we want to grab
//      -return:    a shared_ptr to the component, the shared_ptr will be empty
//                  if the actor does not have the component
//---------------------------------------------------------------------------
StrongActorComponentPtr Actor::GetComponent(const ComponentId componentId) const
{
    auto componentIt = m_components.find(componentId);

    if (componentIt != m_components.end())
    {
        return m_components.find(componentId)->second;
    }
    else
    {
        // failed to find the component
        return StrongActorComponentPtr();
    }
}

//--------------------------------------------------------------------------------------
// Lua Functions
//--------------------------------------------------------------------------------------

void Actor::ExposeToLua(lua_State* pState)
{
    int topOfStack = lua_gettop(pState);

    // Gets a global table from Lua and pushes it on the stack
    lua_getglobal(pState, "Logic");

    // pushes the value of t[k] from the Lua table on the stack
    // so in this case grabs Logic["Actors"] from Lua and pushes it on the stack
    lua_getfield(pState, -1, "Actors");

    // pushes the value onto the stack at the given index
    lua_rawgeti(pState, -1, m_Id);

    // This will expose all of this actor's components to Lua
    for (auto component : m_components)
        component.second->ExposeToLua(pState);

    lua_pop(pState, 3);
    assert(lua_gettop(pState) == topOfStack);
}