#pragma once

#include <map>
#include <memory>
#include <string>

class ActorComponent;
class Actor;

typedef unsigned long ActorId;
typedef int LuaRef;
typedef unsigned long ComponentId;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;