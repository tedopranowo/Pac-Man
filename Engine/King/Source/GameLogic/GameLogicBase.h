//------------------------- 
//Created by Shane King 
//Tue 04/11/2017  3:43:34.17 
//------------------------- 
#pragma once 

#include "..\Actors\Actor.h"
#include "..\EventSystem\TypedefEvents.h"
#include "..\Tiled\Tiled.h"
#include "..\CollisionSystem\CollisionSystem.h"

#include <list>
#include <memory>
#include <vector>
#include "..\Application\Application.h"
#include <lua.hpp>

class GameViewBase;
class Application;
class TransformComponent;

typedef std::vector<std::shared_ptr<GameViewBase>> GameViewList;
typedef std::vector<std::shared_ptr<Actor>> ActorList;

struct MapData
{
public:
	const int m_mapWidth;
	const int m_mapHeight;
	const int m_tileWidth;
	const int m_tileHeight;

	MapData(const int mapWidth = -1, const int mapHeight = -1, 
		const int tileWidth = -1, const int tileHeight = -1)
		: m_mapWidth(mapWidth)
		, m_mapHeight(mapHeight)
		, m_tileWidth(tileWidth)
		, m_tileHeight(tileHeight)
	{}
};

class GameLogicBase
{ 
private:
	const MapData m_mapData;

    ActorId m_actorId;
    Application* m_pGameApp;
    ActorList m_actors;
    GameViewList m_pViews;
    lua_State* m_pLuaState;
    CollisionSystem m_collisionSystem;

    EventListenerDelegate m_actorCreatedDelegate;
	EventListenerDelegate m_actorDestroyedDelegate;

    std::vector<Tiled::TileType> m_tileTypes;
    std::vector<std::vector<Tiled::Tile>> m_map;


    Actor* m_pPawn;

public:
    GameLogicBase(Application* pGameApp);
    ~GameLogicBase();

    void CreateInitialLuaState();
    void Update(double deltatime);
    void SetPawn(Actor* pPawn);
    void PlayBgMusic(bool play);
    void RefreshMap();
    void AttachView(std::shared_ptr<GameViewBase> view);
    void CreateActor(std::string actorName, int x, int y, int srcX, int srcY, int width, int height, const char* filepath);
	void DestroyActor(StrongActorPtr);
    void SetTiles(std::vector<Tiled::TileType>&& types, std::vector<std::vector<Tiled::Tile>>&& tiles);
    bool LoadMap(const char* mapName);
	void DisableTile(int layer, int index);
    std::string GetActorName(const char * filename, ActorId id);
    StrongActorPtr SpawnActor(const char * filename);
    lua_State* GetLuaState() { return m_pLuaState; }
	
	bool IsWall(std::shared_ptr<TransformComponent> pTransform) const;

	const int GetMapWidth() { return m_mapData.m_mapWidth; }
	const int GetMapHeight() { return m_mapData.m_mapHeight; }
	const int GetTileWidth() { return m_mapData.m_tileWidth; }
	const int GetTileHeight() { return m_mapData.m_tileHeight; }

private:
    const ActorId GetActorId() { ++m_actorId; return m_actorId; }
    void ActorCreatedDelegateFunc(EventDataPtr pEventData);
	void ActorDestroyedDelegateFunc(EventDataPtr pEventData);
    void PushActorToLua(StrongActorPtr actor, const char* actorName);
	void UpdateLua(double deltatime);
	void UpdateActors(double deltatime);
	void UpdateViews(double deltatime);
	void CheckWallCollisions();
};