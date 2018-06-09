#pragma once
#include "BaseEventData.h"

#include "../Actors/TypedefActors.h"

class ActorCreatedEvent : public BaseEventData
{
private:
    StrongActorPtr m_actor;

public:
    static const EventType s_kEventType = 0x28179376;

    explicit ActorCreatedEvent(StrongActorPtr actor) : m_actor(actor) {}
    ~ActorCreatedEvent() {}

    virtual const EventType& GetEventType() const override { return s_kEventType; }
    virtual EventDataPtr Copy() const override { return EventDataPtr(std::make_shared<ActorCreatedEvent>(m_actor)); }
    virtual const char* GetName() const { return "ActorCreatedEvent"; }
    const StrongActorPtr GetId() const { return m_actor; }
    const StrongActorPtr GetActor() const { return std::shared_ptr<Actor>(m_actor); }

};