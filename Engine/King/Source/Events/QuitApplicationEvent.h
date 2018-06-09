//------------------------- 
//Created by Shane King 
//Sat 05/13/2017 19:16:09.58 
//------------------------- 
#pragma once

#include "BaseEventData.h"

class QuitApplicationEvent : public BaseEventData
{ 
public: 
    QuitApplicationEvent() {}
    ~QuitApplicationEvent() {}
    
    static const EventType s_kEventType{ 0xa0e2ce84 };

    // Inherited via BaseEventData
    virtual const char * GetName() const override { return "QuitApplicationEvent"; }
    virtual const EventType & GetEventType(void) const override { return s_kEventType; }
    virtual EventDataPtr Copy() const override { return EventDataPtr(); };

};
