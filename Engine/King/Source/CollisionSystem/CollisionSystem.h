// CollisionSystem.h

#pragma once

#include <SDL.h>
#include <queue>
#include <vector>
#include "..\Components\TransformComponent.h"
#include "..\EventSystem\TypedefEvents.h"

typedef std::shared_ptr<TransformComponent> Transform;

class CollisionSystem
{
private:
	std::queue<Transform> m_collisionQueue;
	std::vector<std::shared_ptr<Actor>> *m_pActors;
	lua_State *m_pState;

	EventListenerDelegate m_actorMoveDelegate;

public:
	CollisionSystem(std::vector<std::shared_ptr<Actor>> *pActors, lua_State *pState);
	~CollisionSystem() { m_pActors = nullptr; ClearQueue(); }
    
	void RunCollisionQueue();

private:
	void CheckActorCollision(Transform actorCollider);

	bool CheckCollision(const SDL_Rect &rectA, const SDL_Rect &rectB);
	void AddCollider(Transform collider);
	void ClearQueue();

	void ActorMoveDelegateFunc(EventDataPtr eventData);
};