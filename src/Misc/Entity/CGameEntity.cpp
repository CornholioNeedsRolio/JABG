#include "CGameEntity.h"

#include "Components/CEntityComponent.h"

CGameEntity::CGameEntity(SVector3 _position, SVector3 _rotation) : CObject(_position, _rotation)
{
}

void CGameEntity::Tick(CInputManager& manager, float deltaTime)
{
    for(CEntityComponent* component : EntityComponents)
        component->Tick(deltaTime);
}

void CGameEntity::AddComponent(CEntityComponent* component)
{
    EntityComponents.push_back(component);
}
