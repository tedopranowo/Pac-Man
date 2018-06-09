#pragma once

#include "BaseEventData.h"
#include "..\Actors\TypedefActors.h"

class ActorMoveEvent : public BaseEventData
{
private:
    StrongActorPtr m_actor;

public:
    constexpr static EventType s_kEventType = 0x75eff184;

	explicit ActorMoveEvent(StrongActorPtr actor) : m_actor(actor) {}
	~ActorMoveEvent() {}

	virtual const EventType& GetEventType(void) const override { return s_kEventType; }
	virtual EventDataPtr Copy() const override { return EventDataPtr(std::make_shared<ActorMoveEvent>(m_actor)); }
	virtual const char* GetName() const { return "ActorMoveEvent"; }
	const StrongActorPtr GetActor() const { return std::shared_ptr<Actor>(m_actor); }

};