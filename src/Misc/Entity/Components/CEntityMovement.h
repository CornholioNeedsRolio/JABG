#ifndef CEntityMovement_HEADER
#define CEntityMovement_HEADER
#include <glm/vec3.hpp>

#include "CEntityComponent.h"

class CEntityMovement : public CEntityComponent
{
    glm::vec3 m_inputVector;
    glm::vec3 m_velocity;
    float m_maxSpeed;
    float m_acceletation = 0.1f;
    float m_airaccel = 0.01f;
    float m_gravity = 0.05f;
    float m_friction = 10.f;
    float m_airresistance = 1.f;
    bool m_grounded;

    float m_currentFallVelocity = 0;
    float m_currentSpeed = 0.f;
    bool m_wantsToJump = false;
    
    void ConsumeMovement(float DeltaTime);
    glm::vec3 Accelerate(float DeltaTime, const glm::vec3& accelDir, const glm::vec3& prevVelocity, float accelerate, float max_velocity);
    glm::vec3 MoveGround(float DeltaTime, const glm::vec3& accelDir, glm::vec3 prevVelocity);
    glm::vec3 MoveAir(float DeltaTime, const glm::vec3& accelDir, glm::vec3 prevVelocity);
public:
    CEntityMovement(CGameEntity* Parent);
    virtual void Tick(float DeltaTime) override;
    void AddMovementVector(const glm::vec3& InputVector);
    void SetMaximumWalkingSpeed(float Speed);
    void SetWantsToJump(bool intention);
};

#endif