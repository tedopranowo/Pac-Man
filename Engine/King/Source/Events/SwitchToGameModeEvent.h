#pragma once

#include "BaseEventData.h"

class SwitchToGameModeEvent : public BaseEventData
{
public:
    static const EventType s_kEventType = 0xef7cda0a;

    SwitchToGameModeEvent() {}
    ~SwitchToGameModeEvent() {}

    virtual const char* GetName() const override { return "SwitchToGameMode"; };
    virtual const EventType& GetEventType() const override { return s_kEventType; }
    virtual EventDataPtr Copy() const override { return EventDataPtr(); }
};