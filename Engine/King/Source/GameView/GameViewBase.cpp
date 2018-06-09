#include "GameViewBase.h" 

#include "..\Application\Application.h"
#include "..\ResourceManager\GraphicResource.h"
#include "..\ResourceManager\ResourceHandle.h"
#include "..\Actors\Actor.h"
#include "..\Components\ActorComponent.h"
#include "..\EventSystem\EventManager.h"
#include "..\Events\ActorCreatedEvent.h"
#include "..\Events\ActorDestroyedEvent.h"
#include "..\Events\PanCameraEvent.h"
#include "..\Events\SwitchToFreeFlyModeEvent.h"
#include "..\Events\SwitchToGameModeEvent.h"
#include "..\Events\MapLoadedEvent.h"
#include "..\Components\SquareRenderComponent.h"
#include "..\ResourceManager\ResourceCache.h"

#include <cassert>


#pragma warning(push)
#pragma warning(disable:4100)

using namespace std;

GameViewBase::GameViewBase(Application* pApp, SDL_Renderer* pRen)
    : m_camera(
		Vec2(0, 0),
		Vec2(pApp->m_gameOptions.m_winWidth, pApp->m_gameOptions.m_winHeight), 
		nullptr
	)
    , m_pApp(pApp)
    , m_pRen(pRen)
	, m_pMap(nullptr)
    , m_showColliders(false)
    , m_cameraFree(false)
{   
    MAKE_AND_BIND_DELEGATE(m_mapLoadedDelegate, this, &GameViewBase::MapLoaded, MapLoadedEvent::s_kEventType);
    MAKE_AND_BIND_DELEGATE(m_actorCreatedDelegate, this, &GameViewBase::ActorCreatedDelegateFunc, ActorCreatedEvent::s_kEventType);
	MAKE_AND_BIND_DELEGATE(m_actorDestroyedDelegate, this, &GameViewBase::ActorDestroyedDelegateFunc, ActorDestroyedEvent::s_kEventType);
    MAKE_AND_BIND_DELEGATE(m_panCameraDelegate, this, &GameViewBase::PanCamera, PanCameraEvent::s_kEventType);
    MAKE_AND_BIND_DELEGATE(m_switchToFreeFlyModeDelegate, this, &GameViewBase::SwitchToFreeFlyMode, SwitchToFreeFlyMode::s_kEventType);
    MAKE_AND_BIND_DELEGATE(m_switchToGameModeDelegate, this, &GameViewBase::SwitchToGameMode, SwitchToGameModeEvent::s_kEventType);
}

GameViewBase::~GameViewBase()
{
    m_pRen = nullptr;
    m_pApp = nullptr;
}

/*
This will call the draw function on all the game objects in the view.
The list should not contain any non-renderable objects (this check is done
in ActorCreatedDelegateFunc(), before they are entered into the list)
*/
void GameViewBase::Update(double deltatime)
{
    SDL_RenderClear(m_pRen);
    
    if (!m_cameraFree)
    {
		// This is where we do calculations for the camera to follow an actor
		if (m_camera.targetPos)
		{
			// [TODO] Need to store tilesize.  Need to subtract half of the characters width and height from the denominator
			m_camera.pos.Set(
				m_camera.screenSpace.GetX() / 2 - m_camera.targetPos->GetX(),
				m_camera.screenSpace.GetY() / 2 - m_camera.targetPos->GetY()
				);
		}
    }

    //for ( auto layer : *m_pMap)
    {
		for ( auto tile : (*m_pMap)[1])
		{
			// If it's not an empty tile draw it
			if (tile.m_tileData && tile.enabled)
			{
				// Changing the position in camera space (not world space)
				SDL_Rect dst = tile.m_pos;
				dst.x += (int)m_camera.pos.GetX();
				dst.y += (int)m_camera.pos.GetY();
				SDL_RenderCopy(m_pRen, tile.m_tileData->m_pTexture, &tile.m_tileData->m_src, &dst);
			}
		}
    }

    for (auto object : m_objects)
    {
		std::shared_ptr<SquareRenderComponent> pRenderable = static_pointer_cast<SquareRenderComponent>(object.second);
		pRenderable->Draw(m_pRen, m_camera.pos);
    }
    
#ifdef _DEBUG
	// Only Draw the colliders if we are in free fly mode
	if (m_showColliders)
    {
        for (auto collider : m_colliders)
        {
            collider.second->Draw(m_pRen, m_camera.pos);
        }
    }
#endif // _DEBUG


    SDL_RenderPresent(m_pRen);
}

/*
This will clear the view, typically used before we load a new map
*/
void GameViewBase::ClearView()
{
	if (m_pMap)
	{
		for (unsigned int index{ 0 }; index < m_pMap->size(); ++index)
		{
			m_pMap[index].clear();
		}
	}
#ifdef _DEBUG
	m_colliders.clear();
#endif // _DEBUG

	m_objects.clear();
}

/*
Whenever an actor is created the gameView will check if it has a renderable component.
If it does then it will add it to the game objects list.  Otherwise, does not add it.
*/
void GameViewBase::ActorCreatedDelegateFunc(EventDataPtr eventData)
{
    // Downcast the event to the actor event
    shared_ptr<ActorCreatedEvent> pActorEvent = static_pointer_cast<ActorCreatedEvent>(eventData);
    assert(pActorEvent);

    StrongActorPtr actor = pActorEvent->GetActor();

    StrongActorComponentPtr renderComponent = actor->GetComponent(ActorComponent::SquareRenderComponentID);
    StrongActorComponentPtr boxColliderComponent = actor->GetComponent(ActorComponent::BoxColliderComponentID);

    if (renderComponent)
        m_objects.insert(make_pair(actor->GetActorId(), renderComponent));

#ifdef _DEBUG
    if (boxColliderComponent)
        m_colliders.insert(make_pair(actor->GetActorId(), boxColliderComponent));
#endif // _DEBUG

}

/*
Whenever an actor is destroyed the GameView will remove the corresponding components from it's objects to render
*/
void GameViewBase::ActorDestroyedDelegateFunc(EventDataPtr eventData)
{
	shared_ptr<ActorDestroyedEvent> pActorEvent = static_pointer_cast<ActorDestroyedEvent>(eventData);
	assert(pActorEvent);

	StrongActorPtr actor = pActorEvent->GetActor();

	m_objects.erase(actor->GetActorId());
#ifdef _DEBUG
	m_colliders.erase(actor->GetActorId());
#endif // _DEBUG

}

void GameViewBase::PanCamera(EventDataPtr eventData)
{
    Vec2 panVec;
    shared_ptr<PanCameraEvent> pCamEvent = static_pointer_cast<PanCameraEvent>(eventData);
    assert(pCamEvent);

    const int dir = pCamEvent->GetDirection();

    switch (dir)
    {
    case NORTH:
        panVec = Vec2(0, m_camera.cameraSpeed);
        break;
    case SOUTH:
        panVec = Vec2(0, -m_camera.cameraSpeed);
        break;
    case WEST:
        panVec = Vec2(m_camera.cameraSpeed, 0);
        break;
    case EAST:
        panVec = Vec2(-m_camera.cameraSpeed, 0);
        break;
    default:
        break;
    }
    m_camera.pos = m_camera.pos + panVec;
}

void GameViewBase::SwitchToGameMode(EventDataPtr eventData)
{
    m_cameraFree = false;
    m_showColliders = false;
}

void GameViewBase::SwitchToFreeFlyMode(EventDataPtr eventData)
{
    m_cameraFree = true;
    m_showColliders = true;
}

void GameViewBase::MapLoaded(EventDataPtr eventData)
{   
    shared_ptr<MapLoadedEvent> pLoadedMap = static_pointer_cast<MapLoadedEvent>(eventData);
    vector<Tiled::TileType>* pTileTypes = pLoadedMap->GetTileTypes();

    for (unsigned int index{ 0 }; index < pTileTypes->size(); ++index)
    {
        if ((*pTileTypes)[index].m_pTexture == nullptr)
        {
            // Get the resource cache
            ResourceCache* pCache = m_pApp->GetResourceCache();
            assert(pCache);

            // Get the resource
            shared_ptr<GraphicResource> pResource = static_pointer_cast<GraphicResource>(pCache->GetResource((*pTileTypes)[index].m_filename.c_str())->GetResource());
            assert(pResource);

            // Get the image
            (*pTileTypes)[index].m_pTexture = SDL_CreateTextureFromSurface(m_pRen, pResource->GetGraphic());
            assert((*pTileTypes)[index].m_pTexture);
        }
    }

    m_pMap = pLoadedMap->GetMap();
}
#pragma warning(pop)