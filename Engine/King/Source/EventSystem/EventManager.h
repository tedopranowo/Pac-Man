#pragma once
#include "IEventManager.h"

#include <list>
#include <map>

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager
{
private:
    typedef std::list<EventListenerDelegate> EventListenerList;
    typedef std::map<EventType, EventListenerList> EventListenerMap;
    typedef std::list<EventDataPtr> EventQueue;

    EventListenerMap m_eventListeners;
    EventQueue m_queues[EVENTMANAGER_NUM_QUEUES];
    int m_activeQueue;

public:
    explicit EventManager(const char* pName, bool setAsGlobal);
    virtual ~EventManager() {}

    // Inherited via IEventManager
    virtual bool AddListener(const EventListenerDelegate & eventDelegate, const EventType & type) override;
    virtual bool RemoveListener(const EventListenerDelegate & eventDelegate, const EventType & type) override;
    virtual bool TriggerEvent(const EventDataPtr & pEvent) override;
    virtual bool QueueEvent(const EventDataPtr & pEvent) override;
    virtual bool AbortEvent(const EventType & inType, bool allOfType = false) override;
    virtual bool TickUpdate(unsigned long maxMillis = kINFINITE) override;
};

