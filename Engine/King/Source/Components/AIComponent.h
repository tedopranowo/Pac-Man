#pragma once

#include "ActorComponent.h"

class AIComponent : public ActorComponent
{
private:
    // This id is negative I think (which won't work for unsigned long
    constexpr static ComponentId m_kId{ ActorComponent::AIComponentID};

public:
    AIComponent();
    virtual ~AIComponent();

    // Inherited via ActorComponent
    virtual bool Init(const tinyxml2::XMLElement * pData) override;
    virtual const ComponentId GetComponentId() const override { return m_kId; }
};
