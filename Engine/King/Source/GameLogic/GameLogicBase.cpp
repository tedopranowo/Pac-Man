#include "GameLogicBase.h"

#include "..\GameView\GameViewBase.h"

#include "..\EventSystem\TypedefEvents.h"
#include "..\EventSystem\EventManager.h"
#include "..\Events\ActorCreatedEvent.h"
#include "..\Events\ActorDestroyedEvent.h"
#include "..\Events\MapLoadedEvent.h"
#include "..\ResourceManager\ResourceCache.h"
#include "..\ResourceManager\ResourceHandle.h"
#include "..\ResourceManager\ActorResource.h"
#include "..\ResourceManager\GraphicResource.h"
#include "..\Actors\TypedefActors.h"
#include "..\Components\SquareRenderComponent.h"
#include "..\Components\TransformComponent.h"
#include "..\Components\BoxColliderComponent.h"
#include "..\InputSystem\InputSystem.h"
#include "..\..\..\Logging\Source\Logger.h"
#include "..\Physics\Vec2.h"
#include "..\LuaManager\LuaManager.h"

#include <conio.h>
#include <iostream>
#include <memory>
#include <cassert>

// If true, we can NOT move off the map
#define BOUNDED 1

using namespace std;

// Lua will call and pass in gamelogic(this) and filename for actor
int SpawnActorForLua(lua_State* pState)
{
    /*
    lua_tostring:
    converts the Lua value at the given index to a C string
    returns a pointer to the string inside of the lua stack
    */
    // [0, 0, -]
    std::string resource = lua_tostring(pState, -1);
    
    //lua_tuserdata: if the value at the index is userdata->returns block address, light userdata->returns its pointer, anything else->null
    GameLogicBase* pGameLogic = static_cast<GameLogicBase*>(lua_touserdata(pState, -2));
    
    // pops the number of elements passed in (2 in this case)
    lua_pop(pState, 2);
   
    StrongActorPtr pActor = pGameLogic->SpawnActor(resource.c_str());
    
    // [0, +1, -]
    lua_getglobal(pState, "Logic");
    // [0, +1, e]
    lua_getfield(pState, -1, "Actors");
    // [0, +1, -]
    lua_rawgeti(pState, -1, pActor->GetActorId());
    
    // [-1, 0, -]
    lua_remove(pState, -2);
    // [-1, 0, -]
    lua_remove(pState, -2);
    
    return 1;
}

int DestroyActorForLua(lua_State* pState)
{
    GameLogicBase* pGameLogic = static_cast<GameLogicBase*>(lua_touserdata(pState, -2));
    Actor* pActor = static_cast<Actor*>(lua_touserdata(pState, -1));
    StrongActorPtr toDestroy = make_shared<Actor>(*pActor);
    
    pGameLogic->DestroyActor(toDestroy);
    
    lua_pop(pState, 2);
    
    return 0;
}

// This should take the actor as param1 and the reaction as param2
int AddReactionFunctionFromLua(lua_State *pState)
{
    // param1 the actor
    BoxColliderComponent *pCollider = static_cast<BoxColliderComponent*>(lua_touserdata(pState, -3));
    
    // param2 the id to listen for
    const char *id = lua_tostring(pState, -2);
    
    // param3 is the function to call
    const char *func = lua_tostring(pState, -1);
    
    lua_pop(pState, 3);
    
    std::string idStr = id;
    
    pCollider->AddLuaReaction(idStr, func);
    
    return 0;
}

int GetPositionForLua(lua_State *pState)
{
    TransformComponent *pTransform = static_cast<TransformComponent*>(lua_touserdata(pState, -1));
    
    lua_pop(pState, 1);
    
    Vec2 pos = pTransform->GetPosition();
    
    lua_createtable(pState, 0, 2);
    
    lua_pushinteger(pState, (lua_Integer)pos.m_x);
    
    lua_setfield(pState, -2, "x");
    
    lua_pushinteger(pState, (lua_Integer)pos.m_y);
    
    lua_setfield(pState, -2, "y");
    
    return 1;
}

int DisableTileForLua(lua_State *pState)
{
    GameLogicBase* pGameLogic = static_cast<GameLogicBase*>(lua_touserdata(pState, -3));
    
    int layerIndex = (int)lua_tointeger(pState, -2);
    int tileIndex = (int)lua_tointeger(pState, -1);
    
    pGameLogic->DisableTile(layerIndex, tileIndex);
    
    return 0;
}

int PlayBgMusicForLua(lua_State *pState)
{
    GameLogicBase* pGameLogic = static_cast<GameLogicBase*>(lua_touserdata(pState, -2));

    bool play = lua_toboolean(pState, -1);

    pGameLogic->PlayBgMusic(play);

    return 0;
}

int RefreshMapForLua(lua_State *pState)
{
    GameLogicBase* pGameLogic = static_cast<GameLogicBase*>(lua_touserdata(pState, -1));

    pGameLogic->RefreshMap();

    return 0;
}

/*
Game Logic contains the list of game actors and all the game views
*/
GameLogicBase::GameLogicBase(Application* k_pGameApp)
    : m_pGameApp(k_pGameApp)
    , m_actorId(0)
    , m_pPawn(nullptr)
	, m_pLuaState(luaL_newstate())
    , m_collisionSystem(CollisionSystem(&m_actors, m_pLuaState))
	, m_mapData(Tiled::GetMapData(k_pGameApp->m_gameOptions.m_startMap.c_str()))
{
    MAKE_AND_BIND_DELEGATE(m_actorCreatedDelegate, this, &GameLogicBase::ActorCreatedDelegateFunc, ActorCreatedEvent::s_kEventType);
    MAKE_AND_BIND_DELEGATE(m_actorDestroyedDelegate, this, &GameLogicBase::ActorDestroyedDelegateFunc, ActorDestroyedEvent::s_kEventType);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, spawnActorDescriptor, "SpawnActor", SpawnActorForLua);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, destroyActorDescriptor, "DestroyActor", DestroyActorForLua);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, addReactionDescriptor, "AddReaction", AddReactionFunctionFromLua);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, getPositionDescriptor, "GetPosition", GetPositionForLua);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, disableTileDescriptor, "DisableTile", DisableTileForLua);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, playBgMusicDescriptor, "PlayBgMusic", PlayBgMusicForLua);
    CREATE_AND_ADD_LUA_DESCRIPTOR(m_pGameApp, refreshMapDescriptor, "RefreshMap", RefreshMapForLua);
}

GameLogicBase::~GameLogicBase()
{
    m_pGameApp = nullptr;
    lua_close(m_pLuaState);
    for (auto view : m_pViews)
        view.reset();
    for (auto actor : m_actors)
    {
        actor->Destroy();
    }
    m_actors.clear();
}

void GameLogicBase::CreateInitialLuaState()
{
    // Opens up all of the std Lua libaries into the given state
    luaL_openlibs(m_pLuaState);

    // Creates an empty lua table and pushes it onto the stack
    //[-0, +1, m]
    lua_createtable(m_pLuaState, 0, 3);

    // Pushes a pointer (lightuserdata) onto the stack
    //[-0, +1, -]
    lua_pushlightuserdata(m_pLuaState, this);

    // [-1, 0, e]
    lua_setfield(m_pLuaState, -2, "this");

    // this is pushing a table with 100 array elements [TODO] have level/game options which will state max number of actors in a scene
    // [-0, +1, m]
    lua_createtable(m_pLuaState, 100, 0);

    // [-1, 0, e]
    lua_setfield(m_pLuaState, -2, "Actors");

    // Pop the top element from the stack and set it as a global to with the name of Param2 ("Logic")
    // [-1, 0, e]
    lua_setglobal(m_pLuaState, "Logic");

	// Create a table with 1 nonarray element
    //[-0, +1, m]
    lua_createtable(m_pLuaState, 0, 2);

    // Pushes a pointer (lightuserdata) onto the stack
    //[-0, +1, -]
    lua_pushlightuserdata(m_pLuaState, m_pGameApp->GetInputSystem());
    
    // [-1, 0, e]
    lua_setfield(m_pLuaState, -2, "this");

    // Pop the top element from the stack and set it as a global to with the name of Param2 ("Logic")
    // [-1, 0, e]
    lua_setglobal(m_pLuaState, "InputSystem");

   //Add the declared custom c function to lua
   m_pGameApp->GetLuaManager()->ExposeCustomCFunctionsToLua();
}

/*
Adds a game view to the game logic (views can be human or AI)
*/
void GameLogicBase::AttachView(shared_ptr<GameViewBase> view)
{
    m_pViews.push_back(view);
}

//--------------------------------------------------------------------------------------
// This is the main game logic game loop.  First, it calls the Lua script update.  It then
// checks for collisions.  Followed by updating the actors.  Finally it updates the views,
// which draw the scene.
//--------------------------------------------------------------------------------------
void GameLogicBase::Update(double deltatime)
{
    UpdateLua(deltatime);
    CheckWallCollisions();
    m_collisionSystem.RunCollisionQueue();
    UpdateActors(deltatime);
    UpdateViews(deltatime);
}

//--------------------------------------------------------------------------------------
// Calls the Update() in the Lua script (most likely in main.lua)
//--------------------------------------------------------------------------------------
void GameLogicBase::UpdateLua(double deltatime)
{
	   // Call the Lua script Update()
	   lua_getglobal(m_pLuaState, "LuaUpdate");
	   lua_pushnumber(m_pLuaState, deltatime);
	   if (lua_pcall(m_pLuaState, 1, 0, 0) != 0)
	   {
	       std::string errorMessage = "Failed to call lua_cfunction: ";
	       errorMessage += lua_tostring(m_pLuaState, -1);
	       LOG_ERROR(errorMessage.c_str());
	   }
}

//--------------------------------------------------------------------------------------
// Loops through all of the actors in the game and updates them.
//--------------------------------------------------------------------------------------
void GameLogicBase::UpdateActors(double deltatime)
{
    // Update Actors
    for (auto pActor = m_actors.begin(); pActor != m_actors.end(); ++pActor)
    {
        pActor->get()->Update(deltatime);
    }
}

//--------------------------------------------------------------------------------------
// Updates the views, which it turn draws the scene
//--------------------------------------------------------------------------------------
void GameLogicBase::UpdateViews(double deltatime)
{
    // Update Views
    for (auto viewIterator = m_pViews.begin(); viewIterator != m_pViews.end(); ++viewIterator)
    {
        viewIterator->get()->Update(deltatime);
    }
}

//--------------------------------------------------------------------------------------
// This checks actor to wall collision.  A wall is a static tile in the game world.  If
// an actor will collide with a wall (tile) the actor is "pushed away" from the wall by
// adding a force, opposite to the one that pushed the actor into the wall, onto the actor.
//--------------------------------------------------------------------------------------
void GameLogicBase::CheckWallCollisions()
{
    // Need to figure out a better way to handle this
    ActorList collisionActors = m_actors;

	// Loop through all of the actors
	for (auto pActor = collisionActors.begin(); pActor != collisionActors.end(); ++pActor)
	{
        // Grab the actor's box collider
        StrongActorComponentPtr pColliderBase = pActor->get()->GetComponent(ActorComponent::Id::BoxColliderComponentID);

        // If the actor does not have a box collider we do NOT check for collision
        if (!pColliderBase)
	        continue;

        // Check if it's a trigger
        shared_ptr<BoxColliderComponent> pCollider = static_pointer_cast<BoxColliderComponent>(pColliderBase);
        
        // Don't check for wall collision if it's a trigger
        if (pCollider->IsTrigger())
	        continue;

        // Grab the transform
        StrongActorComponentPtr pTransformBase = pActor->get()->GetComponent(ActorComponent::Id::TransformComponentID);
        shared_ptr<TransformComponent> pTransform = static_pointer_cast<TransformComponent>(pTransformBase);

        // Check if it's hitting a wall
        if (IsWall(pTransform))
        {
	        pCollider->CallLuaReactionWall(m_pLuaState);
        }
	}
}

/*
When a CreateActor call is made, the game logic will create a copy of
the prototype actor and store actor in it's list.
*/
void GameLogicBase::ActorCreatedDelegateFunc(EventDataPtr pEventData)
{
    // Downcast the event to the actor event
    shared_ptr<ActorCreatedEvent> pActorEvent = static_pointer_cast<ActorCreatedEvent>(pEventData);

    // Add the actor to our list of all game actors
    m_actors.push_back(pActorEvent->GetActor());
}

/*
When a DestroyActor call is made, the game logic will find the actor and destroy it.
*/
void GameLogicBase::ActorDestroyedDelegateFunc(EventDataPtr pEventData)
{
    shared_ptr<ActorDestroyedEvent> pActorEvent = static_pointer_cast<ActorDestroyedEvent>(pEventData);
    assert(pActorEvent);
    
    StrongActorPtr toDestroy = pActorEvent->GetActor();
    assert(toDestroy);
    
    ActorId id = toDestroy->GetActorId();
    assert(id);
    
    unsigned int index{ 0 };
    for (auto actor : m_actors)
    {
        if (actor->GetActorId() == id)
        {
            actor->Destroy();
            m_actors.erase(m_actors.begin() + index);
            break;
        }
        ++index;
    }
}

// This doesn't technically destroy the actor, this creates the event to destroy the actor the next frame.
void GameLogicBase::DestroyActor(StrongActorPtr actor)
{
    EventDataPtr pActorDestroyed = make_shared<ActorDestroyedEvent>(actor);
    EventManager::Get()->QueueEvent(pActorDestroyed);
}

// This is called when an actor is created from Lua and spawned in the map
StrongActorPtr GameLogicBase::SpawnActor(const char* filename)
{
    // Get the resource cache
    ResourceCache* const pCache = m_pGameApp->GetResourceCache();

    // Grab the resource from the resource handle
    shared_ptr<Resource> resource = pCache->GetResource(filename)->GetResource();

    // Downcast the actor resource
    shared_ptr<ActorResource> pActorResource = static_pointer_cast<ActorResource>(resource);

    // Get the actor and create a copy of it
    StrongActorPtr toAdd = pActorResource->GetActor()->Clone();

    // Add the actor to the game world
    toAdd->SetActorId(GetActorId());
    
    // Push the actor to Lua
    PushActorToLua(toAdd, filename);

    // Queue actor created event for event delegates
    EventDataPtr pActorCreated = make_shared<ActorCreatedEvent>(toAdd);
    EventManager::Get()->TriggerEvent(pActorCreated);
    //EventManager::Get()->QueueEvent(pActorCreated);

    // return the actor (if called from GameLogicLua::CreateActor, it will use the actor)
    return toAdd;
}

//------------------------------------------------------------------------------------
// This will check the collision layer (which is always the last vector (layer) of the map
// and check if there is a valid Tile.  This is a very quick look up because it just
// calculates the 4 points and indexes into the collision layer array.
//		
//		-return: True, if there is a valid Tile (a wall).  False if there is no wall
//------------------------------------------------------------------------------------
bool GameLogicBase::IsWall(shared_ptr<TransformComponent> pTransform) const
{
    // The collision layer is always the last layer
    size_t collisionLayerIndex = m_map.size() - 1;
    
    // Get the x and y of the Vector
    const int x = (int)pTransform->GetPosition().GetX();
    const int y = (int)pTransform->GetPosition().GetY();
    const int w = (int)pTransform->GetWidth();
    const int h = (int)pTransform->GetHeight();
    
    // Create an array of the 4 points of the sprite
    Vec2 points[4] =
    {
        Vec2(x, y),			// top-left
        Vec2(x + w, y),		// top-right
        Vec2(x, y + h),		// bottom-left
        Vec2(x + w, y + h)	// bottom-right
    };
    
    // Calculate the point in tiled space
    for (auto point : points)
    {
        double pntX = point.GetX();
        double pntY = point.GetY();
        
        if (pntX < 0 || pntY < 0)
        	return BOUNDED;
        
        // Calculate the X and Y tile positions
        const int xTile = (int)point.GetX() / m_mapData.m_tileWidth;
        const int yTile = (int)point.GetY() / m_mapData.m_tileHeight;
        
        // calculate the tile index (m_mapWidth is in tiles)
        unsigned int tileIndex = (yTile * m_mapData.m_mapWidth) + xTile;
        
        // make sure we aren't out of bounds
        //if (tileIndex > m_map[collisionLayerIndex].size() || tileIndex < 0)
        //	return BOUNDED;
        
        if (tileIndex < m_map[collisionLayerIndex].size() && tileIndex > 0)
        {
            // If there's tile data then we've hit a tile on the collision layer
            if (m_map[collisionLayerIndex][tileIndex].m_tileData && m_map[collisionLayerIndex][tileIndex].enabled)
            {
        	    pTransform->MoveBack(m_map[collisionLayerIndex][tileIndex].m_pos);
        	    return true;
            }
        }
    }
    
    // if we've reached here then we haven't hit a wall (tile on collision layer)
    return false;
}

// This is called when an actor is created from a Tiled map
void GameLogicBase::CreateActor(string actorName, int x, int y, int srcX, int srcY, int width, int height, const char* filepath)
{
	// IMPORTANT: Tiled X, Y is at the bottom left, need to adjust
	y = y - height;

    // Get the resource cache
    ResourceCache* const pCache = m_pGameApp->GetResourceCache();
    assert(pCache);

    // Grab the resource
    shared_ptr<Resource> resource = pCache->GetResource(actorName.c_str())->GetResource();
    assert(resource);

    // Downcast to actor resource
    shared_ptr<ActorResource> pActorResource = static_pointer_cast<ActorResource>(resource);

    // Get the actor and create a copy of it
    shared_ptr<Actor> toAdd = pActorResource->GetActor()->Clone();

    // Add the actor into our game world
    toAdd->SetActorId(GetActorId());
    
    // Grab the actor's transform component
    StrongActorComponentPtr pComponent = toAdd->GetComponent(ActorComponent::TransformComponentID);

    // Transform component to add
    shared_ptr<TransformComponent> pTransform;

    // The actor has a transform component
    if (pComponent)
        pTransform = static_pointer_cast<TransformComponent>(pComponent);

    // The actor does NOT have a transform component.
    else
        LOG_ERROR("Attempting to create an actor from Tiled without a Transform Component.  Actors in created in Tiled MUST have a Transform Component.");
    
    // Set the actor's position
    pTransform->SetPosition(x, y);

	// If the actor does not have a Square Render Component, automatically generate it based on the xml file
    if (toAdd->HasComponent(ActorComponent::SquareRenderComponentID) == false)
    {
        // Grab the image resource
        shared_ptr<ResourceHandle> pResourceHandle = pCache->GetResource(filepath);

        // Downcast the Image Resource
        shared_ptr<GraphicResource> pImageResource = static_pointer_cast<GraphicResource>(pResourceHandle->GetResource());

        // Get the image
        SDL_Surface* pImage = pImageResource->GetGraphic();

        // Create the actor's renderable component
        SquareRenderComponent renderComponent = SquareRenderComponent(srcX, srcY, width, height, pImage);
        StrongActorComponentPtr renderPtr = make_shared<SquareRenderComponent>(renderComponent);

        // Add the component to the actor
        toAdd->AddComponent(toAdd, renderPtr);
    }

    // Queue create actor event
    EventDataPtr pActorCreated = make_shared<ActorCreatedEvent>(toAdd);
    
    // Push the actor to Lua so actor can be used in scripts
    PushActorToLua(toAdd, actorName.c_str());
    
    EventManager::Get()->TriggerEvent(pActorCreated);
}

void GameLogicBase::PushActorToLua(StrongActorPtr actor, const char* actorName)
{
    lua_getglobal(m_pLuaState, "Logic");
    lua_getfield(m_pLuaState, -1, "Actors");

    // pushes value onto the stack
    lua_pushinteger(m_pLuaState, actor->GetActorId());

    // Create the actor table // [TODO] (with 4 elements (better way to handle this??)
    lua_createtable(m_pLuaState, 0, 4);

	// Get a reference to the lua table (which pops it from the stack)
	LuaRef luaRef = luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
	assert(luaRef != LUA_REFNIL);
	   
	// Store this actor's reference to lua table as a member variable
	actor->SetLuaRef(luaRef);

	// push the table back onto the stack
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, luaRef);

    // Create pointer to the logic
    lua_pushlightuserdata(m_pLuaState, actor.get());
    lua_setfield(m_pLuaState, -2, "this");

    lua_pushstring(m_pLuaState, GetActorName(actorName, actor.get()->GetActorId()).c_str());
    lua_setfield(m_pLuaState, -2, "name");

    lua_settable(m_pLuaState, -3);

    lua_remove(m_pLuaState, -2);
    lua_remove(m_pLuaState, -2);
}

void GameLogicBase::SetTiles(vector<Tiled::TileType>&& types, vector<vector<Tiled::Tile>>&& tiles)
{
    m_tileTypes = move(types);
    m_map       = move(tiles);
}

bool GameLogicBase::LoadMap(const char* mapName)
{
	m_tileTypes.clear();
	m_map.clear();
	for (auto view : m_pViews)
	{
	    view->ClearView();
	}

    if (Tiled::Load(mapName, *this))
    {
        EventDataPtr pMapLoaded = make_shared<MapLoadedEvent>(&m_tileTypes, &m_map);
        EventManager::Get()->QueueEvent(pMapLoaded);
        return true;
    }
    
    return false;
}

void GameLogicBase::DisableTile(int layer, int index)
{
    if (index > 0)
        m_map[layer][index].enabled = false;
}

string GameLogicBase::GetActorName(const char* filename, ActorId id)
{
    string actorName    = filename;
    size_t startPos     = actorName.find_last_of("\\");
    size_t endPos       = actorName.find_last_of(".");
    size_t count        = endPos - startPos;

    actorName = actorName.substr(startPos + 1, count - 1).c_str();
    actorName.append("_");
    actorName.append(to_string(id));
    return actorName;
}

void GameLogicBase::SetPawn(Actor* pPawn)
{
    m_pPawn = pPawn;
}

//------------------------------------------------------------------------------------
// This plays or pauses the background music
//------------------------------------------------------------------------------------
void GameLogicBase::PlayBgMusic(bool play)
{
    m_pGameApp->PlayBgMusic(play);
}

//------------------------------------------------------------------------------------
// This restores all of the map tiles that have been disabled on the collision layer
//  TODO: loop through all of the layers (not just collision layer)
//------------------------------------------------------------------------------------
void GameLogicBase::RefreshMap()
{
    for (unsigned int i{ 0 }; i < m_map[1].size(); ++i)
    {
        if (m_map[1][i].m_tileData)
        {
            if (m_map[1][i].enabled == false)
            {
                m_map[1][i].enabled = true;
            }
        }
    }
}