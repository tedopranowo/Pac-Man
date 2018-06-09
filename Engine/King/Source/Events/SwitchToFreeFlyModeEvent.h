#pragma once

#include "BaseEventData.h"

class SwitchToFreeFlyMode : public BaseEventData
{
public:
    static const EventType s_kEventType = 0x963fca82;

    SwitchToFreeFlyMode() {}
    ~SwitchToFreeFlyMode() {}

    virtual const char* GetName() const override { return "SwitchToFreeFlyMode"; };
    virtual const EventType& GetEventType() const override { return s_kEventType; }
    virtual EventDataPtr Copy() const override { return EventDataPtr(); }
};