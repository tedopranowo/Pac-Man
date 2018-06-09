#pragma once

#include "..\EventSystem\TypedefEvents.h"

class EventData
{
public:
    virtual const EventType& GetEventType() const = 0;
    virtual float GetTimeStamp() const = 0;
    virtual EventDataPtr Copy() const = 0;
    virtual const char* GetName() const = 0;
};