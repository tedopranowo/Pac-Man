// CollisionSystem.cpp

#include "CollisionSystem.h"
#include "..\Physics\Vec2.h"
#include "..\Components\BoxColliderComponent.h"
#include "..\EventSystem\EventManager.h"
#include "..\Events\ActorMoveEvent.h"

#include <cassert>

using namespace std;

CollisionSystem::CollisionSystem(std::vector<std::shared_ptr<Actor>> *pActors, lua_State *pState)
	: m_pActors(pActors)
	, m_pState(pState)
{
	MAKE_AND_BIND_DELEGATE(m_actorMoveDelegate, this, &CollisionSystem::ActorMoveDelegateFunc, ActorMoveEvent::s_kEventType);
}

void CollisionSystem::RunCollisionQueue()
{
	while (!m_collisionQueue.empty())
	{
        Transform toCheck = m_collisionQueue.front();
		CheckActorCollision(toCheck);
		m_collisionQueue.pop();
	}
}

void CollisionSystem::CheckActorCollision(Transform actorTransform)
{
    StrongActorPtr pActor = actorTransform->GetOwner();

    if (pActor->HasComponent(ActorComponent::Id::BoxColliderComponentID))
    {
        std::vector<std::shared_ptr<Actor>> collisionActors = *m_pActors;

        // Check Collisions
        for (auto pOtherActor = collisionActors.begin(); pOtherActor != collisionActors.end(); ++pOtherActor)
        {
            // make sure it's not checking against itself
            if (actorTransform->GetOwner().get() == pOtherActor->get())
                continue;

            // Grab the actor's collider component
            StrongActorComponentPtr pColliderBase = pOtherActor->get()->GetComponent(ActorComponent::Id::BoxColliderComponentID);

            // Actor does not have a collider component
            if (!pColliderBase)
                continue;

            // Downcast the collider components
            shared_ptr<BoxColliderComponent> pCollider = static_pointer_cast<BoxColliderComponent>(pColliderBase);

            // Grab the transform
            StrongActorComponentPtr pOtherTransformBase = pOtherActor->get()->GetComponent(ActorComponent::Id::TransformComponentID);

            shared_ptr<TransformComponent> pOtherTransform = static_pointer_cast<TransformComponent>(pOtherTransformBase);

            SDL_Rect pActorRect = actorTransform->GetRect();
            SDL_Rect pColliderRect = pOtherTransform->GetRect();

            bool collided = CheckCollision(pActorRect, pColliderRect);

            if (collided)
            {
                if (!pCollider->IsTrigger())
                {
                    actorTransform->MoveBack(pOtherTransform->GetRect());
                }

                // Get the actor's box collider
                shared_ptr<BoxColliderComponent> pActorCollider = static_pointer_cast<BoxColliderComponent>(pActor->GetComponent(ActorComponent::Id::BoxColliderComponentID));

                // Get the actors' references to lua tables
                LuaRef otherRef = pOtherActor->get()->GetLuaRef();

                // Call the lua reaction function
                pActorCollider->CallLuaReaction(m_pState, pCollider->GetReactId().c_str(), otherRef);
            }
        }
    }
}

bool CollisionSystem::CheckCollision(const SDL_Rect &rectA, const SDL_Rect &rectB)
{
	// AABB v AABB collision
	int minXa = rectA.x;
	int minYa = rectA.y;
	int maxXa = rectA.x + rectA.w;
	int maxYa = rectA.y + rectA.h;
					 
	int minXb = rectB.x;
	int minYb = rectB.y;
	int maxXb = rectB.x + rectB.w;
	int maxYb = rectB.y + rectB.h;

	return !((maxXa < minXb) || (maxXb < minXa) || (maxYa < minYb) || (maxYb < minYa));
}

void CollisionSystem::AddCollider(Transform collider)
{
	m_collisionQueue.push(collider);
}

void CollisionSystem::ClearQueue()
{
	std::queue<Transform> empty;
	m_collisionQueue = empty;
}

void CollisionSystem::ActorMoveDelegateFunc(EventDataPtr eventData)
{
	shared_ptr<ActorMoveEvent> pMoveEvent = static_pointer_cast<ActorMoveEvent>(eventData);
	if (!pMoveEvent)
		return;

	StrongActorPtr pActor = pMoveEvent->GetActor();

	StrongActorComponentPtr pTransform = pActor->GetComponent(ActorComponent::TransformComponentID);
	if (!pTransform)
		return;

	m_collisionQueue.push(static_pointer_cast<TransformComponent>(pTransform));
}
