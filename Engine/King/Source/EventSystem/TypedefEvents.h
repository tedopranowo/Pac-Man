#pragma once
#include <memory>

#pragma warning(push)
#pragma warning(disable:4100)
	#include "..\FastDelegate\FastDelegate.h"
#pragma warning(pop)

#define MAKE_AND_BIND_DELEGATE(delegateObject, delegateOwner, func, eventType) delegateObject = fastdelegate::MakeDelegate(delegateOwner, func); \
    EventManager::Get()->AddListener(delegateObject, eventType);

class EventData;

typedef unsigned long EventType;
typedef std::shared_ptr<EventData> EventDataPtr;
// FastDelegate1 binds runtime object with function pointer..
// ..the template param will be passed as the param for the function
typedef fastdelegate::FastDelegate1<EventDataPtr> EventListenerDelegate; 