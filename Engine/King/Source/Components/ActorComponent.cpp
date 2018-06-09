#include "ActorComponent.h"

ActorComponent::ActorComponent()
    : m_pOwner(nullptr)
{
}

void ActorComponent::RemoveOwner()
{
    if (m_pOwner != nullptr)
        m_pOwner.reset();
}
