#pragma once
#include "EventData.h"

class BaseEventData : public EventData
{
private:
    const float m_timeStamp;

public:
    BaseEventData(const float timeStamp = 0.0)
        : m_timeStamp(timeStamp) {}
    virtual ~BaseEventData() {}

    // Returns the type of the event
    virtual const EventType& GetEventType(void) const = 0;
    virtual EventDataPtr Copy() const = 0;
    float GetTimeStamp() const { return m_timeStamp; }
};