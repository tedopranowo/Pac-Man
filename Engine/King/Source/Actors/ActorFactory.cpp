// ActorFactory.cpp

#include "ActorFactory.h"

#include "Actor.h"
#include "../Components/SquareRenderComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../../../Logging/Source/Logger.h"

//--------------------------------------------------------------------------------------
//  The ActorFactory and ResourceCache combination follows the prototype pattern.  The
//  ActorFactory should only be used by the ResourceCache.  Nowhere else in the
//  engine should an Actor or a Component be created (not including cloning Actors and
//  Components that already exist).  When creating a brand new Actor, the Actor should
//  be created by the ActorFactory and put into the ResourceCache.  Then get a copy of
//  the Actor Resource from the ResourceCache and put the Actor into the game world.
//      -ActorComponentCreatorMap: is a map of every ActorComponent.
//          -key: the component name (string)
//          -value: a function pointer, where the function return a shared_ptr to the component
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Component Creator Functions
//--------------------------------------------------------------------------------------
StrongActorComponentPtr CreateSquareRenderComponent()
{
    return std::make_shared<SquareRenderComponent>();
}

StrongActorComponentPtr CreateBoxColliderComponent()
{
    return std::make_shared<BoxColliderComponent>();
}

StrongActorComponentPtr CreateTransformComponent()
{
    return std::make_shared<TransformComponent>();
}

//--------------------------------------------------------------------------------------
//  The actor factory is only used by the resource cache.  The resource cache
//  will store the actual prototypes which the game logic will make copies of.
//--------------------------------------------------------------------------------------
ActorFactory::ActorFactory()
{
    // Bind the function pointers which create actor components
    m_ActorComponentCreators["SquareRenderComponent"]   = CreateSquareRenderComponent;
    m_ActorComponentCreators["BoxColliderComponent"]    = CreateBoxColliderComponent;
    m_ActorComponentCreators["TransformComponent"]      = CreateTransformComponent;
}

//--------------------------------------------------------------------------------------
//  This is the function that actually creates a new type of Actor for the game.
//      -actorResource:     this is the filename of the actor data (stored via xml)
//      -return:    a strong pointer to the actor, nullptr if we failed to load the actor
//--------------------------------------------------------------------------------------
StrongActorPtr ActorFactory::CreateActor(const char* actorResource)
{
    // The actor data
    tinyxml2::XMLDocument actor;
    
    // Load the actor data
    if (actor.LoadFile(actorResource) != 0)
    {
        // Unable to load the actor data
        LOG_ERROR("Load Actor XML Error: see below explanation.");
        actor.PrintError();
        return nullptr;
    }

    // Create an empty Actor
    StrongActorPtr pActor = std::shared_ptr<Actor>(std::make_shared<Actor>());
    
    // Get the root element of the actor xml
    const tinyxml2::XMLElement* pRoot = actor.RootElement();

    // loop through each element of the xml.  Each element stores ActorComponent data.
    for (const tinyxml2::XMLElement* component = pRoot->FirstChildElement();
        component != 0; component = component->NextSiblingElement())
    {
        // Create the Component
        StrongActorComponentPtr pActorComponent = CreateComponent(component);
        
        // Add the component to the actor
        pActor->AddComponent(pActor, pActorComponent);
    }
    
    // return the actor prototype
	return pActor;
}

//--------------------------------------------------------------------------------------
//  Creates the actor component by calling the appropriate component creator function
//      -pData: the component data
//      -return: shared_ptr to the component
//--------------------------------------------------------------------------------------
StrongActorComponentPtr ActorFactory::CreateComponent(const tinyxml2::XMLElement* pData)
{
    // Grab the Component Creator function 
    auto componentIt = m_ActorComponentCreators.find(pData->Value());
    StrongActorComponentPtr(*creator)(void) = componentIt->second;

    // Create the component
    StrongActorComponentPtr pComponent  = StrongActorComponentPtr(creator());

    // Initialize the component with the data from the Actor xml file
    pComponent->Init(pData);

    // return a shared_ptr to the component
    return pComponent;
}

