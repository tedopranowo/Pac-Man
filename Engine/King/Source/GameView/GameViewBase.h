//------------------------- 
//Created by Shane King 
//Tue 04/11/2017  3:51:25.33 
//------------------------- 
#pragma once 

#include <map>

#include "..\..\SDL2-2.0.5\include\SDL.h"
#include "..\..\..\King\Source\Physics\Vec2.h"
#include "..\..\..\King\Source\Actors\TypedefActors.h"
#include "..\..\..\King\Source\EventSystem\TypedefEvents.h"
#include "..\..\..\King\Source\Tiled\Tiled.h"

class Application;

class GameViewBase
{
private:

    struct Camera
    {
        constexpr static int cameraSpeed{ 30 };

        Vec2 pos;
        Vec2 screenSpace;
        Vec2* targetPos;

        Camera() {}
        Camera(Vec2 position, Vec2 screenSpace, Vec2* target) : pos(position), screenSpace(screenSpace), targetPos(target) {}
    };

    Camera m_camera;

	/* Private Variables */
    Application* m_pApp;
    SDL_Renderer* m_pRen;

    std::vector<std::vector<Tiled::Tile>>* m_pMap;
#ifdef _DEBUG
    std::map<ActorId, std::shared_ptr<ActorComponent>> m_colliders;
#endif // _DEBUG

    std::map<ActorId, std::shared_ptr<ActorComponent>> m_objects;

    EventListenerDelegate m_mapLoadedDelegate;
    EventListenerDelegate m_actorCreatedDelegate;
	EventListenerDelegate m_actorDestroyedDelegate;
    EventListenerDelegate m_panCameraDelegate;
    EventListenerDelegate m_switchToGameModeDelegate;
    EventListenerDelegate m_switchToFreeFlyModeDelegate;


    bool m_showColliders;
    bool m_cameraFree;

public:
	/* Public Methods */
    GameViewBase(Application* pApp, SDL_Renderer* pRen);
    ~GameViewBase();

    void Update(double deltatime);
    void SetCameraTarget(Vec2* target) { m_camera.targetPos = target; }
    void ReleaseCameraTarget() { m_camera.targetPos = nullptr; }
	void ClearView();

private:
    void MapLoaded(EventDataPtr eventData);
    void ActorCreatedDelegateFunc(EventDataPtr eventData);
	void ActorDestroyedDelegateFunc(EventDataPtr eventData);
    void PanCamera(EventDataPtr eventData);
    void SwitchToGameMode(EventDataPtr eventData);
    void SwitchToFreeFlyMode(EventDataPtr eventData);

}; 