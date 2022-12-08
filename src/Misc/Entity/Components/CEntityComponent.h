#ifndef IEntityComponent_HEADER
#define IEntityComponent_HEADER
#include <memory>

class CEntityComponent : std::enable_shared_from_this<CEntityComponent>
{
private:
    class CGameEntity* m_parent = nullptr;
protected:
    CGameEntity* GetParent() const;
public:
    virtual ~CEntityComponent() = default;
    CEntityComponent(CGameEntity* Parent);
    CEntityComponent() = delete;
    virtual void Tick(float DeltaTime) = 0;

};

#endif