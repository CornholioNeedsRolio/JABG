#ifndef IEntity_HEADER
#define IEntity_HEADER
#include <vector>
#include "Engine/Objects/CObject.h"

class CGameEntity : public CObject
{
    std::vector<class CEntityComponent*> EntityComponents;
public:
    CGameEntity(SVector3 _position = SVector3(0), SVector3 _rotation = SVector3(0));
    virtual void Tick(CInputManager& manager, float deltaTime);
    virtual void AddComponent(class CEntityComponent* component);


    virtual class CWorld* GetWorld() const = 0;
    virtual class CGameEntityCollider* GetCollider() const = 0;
};

#endif
