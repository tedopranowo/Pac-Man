#pragma once

#include "BaseEventData.h"

#include "../Actors/TypedefActors.h"

class ActorDestroyedEvent : public BaseEventData
{
private:
	StrongActorPtr m_actor;

public:
	static const EventType s_kEventType = 0x938f812a;

	explicit ActorDestroyedEvent(StrongActorPtr actor) : m_actor(actor) {}
	~ActorDestroyedEvent() {}

	virtual const EventType& GetEventType() const override { return s_kEventType; }
	virtual EventDataPtr Copy() const override { return EventDataPtr(std::make_shared<ActorDestroyedEvent>(m_actor)); }
	virtual const char* GetName() const { return "ActorDestroyedEvent"; }
	const StrongActorPtr GetId() const { return m_actor; }
	const StrongActorPtr GetActor() const { return std::shared_ptr<Actor>(m_actor); }
};