#pragma once

#include "BaseEventData.h"

#include "..\Actors\TypedefActors.h"

class RenderableCreatedEvent : public BaseEventData
{
private:
    StrongActorComponentPtr m_pComponent;

public:
    static const EventType s_kEventType = 0x5a0dcb39;
    
    explicit RenderableCreatedEvent(StrongActorComponentPtr pComponent) : m_pComponent(pComponent) {}
    ~RenderableCreatedEvent() {}

    // Inherited via BaseEventData
    virtual const char * GetName() const override { return "RenderableCreatedEvent"; };
    virtual const EventType & GetEventType(void) const override { return s_kEventType; };
    virtual EventDataPtr Copy() const override { return EventDataPtr(std::make_shared<RenderableCreatedEvent>(m_pComponent)); }
    const StrongActorComponentPtr GetComponent() const { return m_pComponent; }
};