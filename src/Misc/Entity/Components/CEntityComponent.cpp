#include "CEntityComponent.h"
#include "Misc/Entity/CGameEntity.h"

CGameEntity* CEntityComponent::GetParent() const
{
    return m_parent;
}

CEntityComponent::CEntityComponent(CGameEntity* Parent) : m_parent(Parent)
{
    Parent->AddComponent(this);
}
