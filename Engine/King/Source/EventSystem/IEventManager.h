#pragma once

#include "TypedefEvents.h"

class IEventManager
{
public:

    enum Constants { kINFINITE = 0xffffffff };

    explicit IEventManager(const char* pName, bool setAsGlobal);
    virtual ~IEventManager();

    // Adds a delegate function that will get called when the event type is triggered..
    // ..true = success, false = fail
    virtual bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

    // Removes a delegate/eventType pairing.  False = pair not found
    virtual bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

    // Bypass the queue and trigger this event and all it's listeners NOW
    virtual bool TriggerEvent(const EventDataPtr& pEvent) = 0;

    // Triggers the next event in the queue
    virtual bool QueueEvent(const EventDataPtr& pEvent) = 0;

    // finds the next instance of the event and removes it from the queue (can occur during the call of the event itself)..
    // ..if allOfTime is true, all events of that type are removed.  true = found and removed, false = not removed
    virtual bool AbortEvent(const EventType& inType, bool allOfType = false) = 0;

    // May set the time allowed to process events.  True = all messages were completed, false = otherwise
    virtual bool TickUpdate(unsigned long maxMillis = kINFINITE) = 0;

    // Returns a pointer to the singleton event manager
    static IEventManager* Get();
};


