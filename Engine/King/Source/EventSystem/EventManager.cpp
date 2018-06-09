#include "EventManager.h"

#include <assert.h>
#include <windows.h>

#include "..\Events\EventData.h"
#include "..\..\..\Logging\Source\Logger.h"

EventManager::EventManager(char const* pName, bool setAsGlobal)
    : IEventManager(pName, setAsGlobal)
{
    m_activeQueue = 0;
}

/*
This takes in the delegate and the EventType, which is a static member of every Event.
Example of 2nd parameter ActorCreatedEvent::sk_EventType
*/
bool EventManager::AddListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    EventListenerList& eventListenerList = m_eventListeners[type];
    for (auto eventIt = eventListenerList.begin(); eventIt != eventListenerList.end(); ++eventIt)
    {
        if (eventDelegate == (*eventIt))
        {
            LOG_ERROR("Attempting to add listener twice");
            return false;
        }
    }

    eventListenerList.push_back(eventDelegate);
    return true;
}

/*
Removes an event listener (delegate / pointer to a function) to a specific event type 
from the list
*/
bool EventManager::RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    bool success = false;

    auto findEventIt = m_eventListeners.find(type);
    if (findEventIt != m_eventListeners.end())
    {
        EventListenerList& listeners = findEventIt->second;
        for (auto eventIt = listeners.begin(); eventIt != listeners.end(); ++eventIt)
        {
            if (eventDelegate == (*eventIt))
            {
                listeners.erase(eventIt);
                success = true;
                break;
            }
        }
    }
    
    return success;
    return false;
}

/*
This will trigger an event immediately and all of the listeners to that event
*/
bool EventManager::TriggerEvent(const EventDataPtr& pEvent)
{
    bool processed = false;

    auto findEventIt = m_eventListeners.find(pEvent->GetEventType());
    if (findEventIt != m_eventListeners.end())
    {
        const EventListenerList& eventListenerList = findEventIt->second;
        for (EventListenerList::const_iterator eventIt = eventListenerList.begin(); eventIt != eventListenerList.end(); ++eventIt)
        {
            EventListenerDelegate listener = (*eventIt);
            listener(pEvent);
            processed = true;
        }
    }

    return processed;
}

/*
Add an event to the event queue.
The event will NOT be added to the queue if there are no event listeners for it.
*/
bool EventManager::QueueEvent(const EventDataPtr & pEvent)
{
    assert(m_activeQueue >= 0);
    assert(m_activeQueue < EVENTMANAGER_NUM_QUEUES);

    auto findEventIt = m_eventListeners.find(pEvent->GetEventType());
    if (findEventIt != m_eventListeners.end())
    {
        m_queues[m_activeQueue].push_back(pEvent);
        return true;
    }
    else
    {
        return false;
    }
}

/*
Remove an event from the queue
*/
bool EventManager::AbortEvent(const EventType& inType, bool allOfType)
{
    assert(m_activeQueue >= 0);
    assert(m_activeQueue < EVENTMANAGER_NUM_QUEUES);
    
    bool success = false;
    EventListenerMap::iterator findEventIt= m_eventListeners.find(inType);

    if (findEventIt != m_eventListeners.end())
    {
        EventQueue& eventQueue = m_queues[m_activeQueue];
        auto eventIt = eventQueue.begin();
        while (eventIt != eventQueue.end())
        {
            auto thisIt = eventIt;
            ++eventIt;
            if ((*thisIt)->GetEventType() == inType)
            {
                eventQueue.erase(thisIt);
                success = true;
                if (!allOfType)
                {
                    break;
                }
            }
        }
    }
    return success;
}

/*
Call the next processes in the queue for 'x' amount of time, then setup queue for next update.
*/
bool EventManager::TickUpdate(unsigned long maxMillis)
{
    unsigned long currMs = GetTickCount();
    unsigned long maxMs = ((maxMillis == IEventManager::kINFINITE)) ? (IEventManager::kINFINITE) : (currMs + maxMillis);

    int queueToProcess = m_activeQueue;
    m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUES;
    m_queues[m_activeQueue].clear();

    // Process the queue
    while (!m_queues[queueToProcess].empty())
    {
        // pop
        EventDataPtr pEvent = m_queues[queueToProcess].front();
        m_queues[queueToProcess].pop_front();

        const EventType& eventType = pEvent->GetEventType();

        // find delegate for this event type
        auto findEventIt = m_eventListeners.find(eventType);
        if (findEventIt != m_eventListeners.end())
        {
            const EventListenerList& eventListeners = findEventIt->second;

            for (auto eventIt = eventListeners.begin(); eventIt != eventListeners.end(); ++eventIt)
            {
                EventListenerDelegate listener = (*eventIt);
                listener(pEvent);
            }
        }

        // Check to see if time ran out
        currMs = GetTickCount();
        if (maxMillis != IEventManager::kINFINITE && currMs >= maxMs)
        {
            LOG("Event Loop", "Aborting event processing, time ran out", Logger::CautionLevel::None);
            break;
        }
    }

    // if unable to process all events, push the remaining events to the new active queue, back to front
    bool queueFlushed = (m_queues[queueToProcess].empty());
    if (!queueFlushed)
    {
        while (!m_queues[queueToProcess].empty())
        {
            EventDataPtr pEvent = m_queues[queueToProcess].back();
            m_queues[queueToProcess].pop_back();
            m_queues[m_activeQueue].push_front(pEvent);
        }
    }
    return queueFlushed;
}
