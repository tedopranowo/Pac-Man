#pragma once

#include "TypedefActors.h"
#include "../Xml/tinyxml2.h"

typedef StrongActorComponentPtr(* ActorComponentCreator)(void);
typedef std::map<std::string, ActorComponentCreator> ActorComponentCreatorMap;

class ActorFactory
{
private:
	ActorComponentCreatorMap m_ActorComponentCreators;

public:
    ActorFactory();
    ~ActorFactory() {}

	StrongActorPtr CreateActor(const char* actorResource);

protected:
	StrongActorComponentPtr CreateComponent(const tinyxml2::XMLElement* pData);

};

