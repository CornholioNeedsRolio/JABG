﻿#include "CEntityMovement.h"

#include <iostream>

#include "Game/GameWorld/CWorld.h"
#include <glm/gtx/string_cast.hpp>
#include "Misc/Entity/CGameEntity.h"

void CEntityMovement::Tick(float DeltaTime)
{
    ConsumeMovement(DeltaTime);
}

void CEntityMovement::ConsumeMovement(float DeltaTime)
{
    float CurrentSpeed = glm::length(m_velocity);
    m_currentSpeed = std::min(m_maxSpeed, CurrentSpeed+DeltaTime*m_acceletation*0.00001f);
    if(glm::length(m_inputVector) > 0)
        m_inputVector = glm::normalize(m_inputVector);

    if(m_grounded)
        m_velocity = MoveGround(DeltaTime, m_inputVector, m_velocity);
    else
        m_velocity = MoveAir(DeltaTime, m_inputVector, m_velocity);
    float FallMultiplier = 1.f;
    if(m_velocity.y <= 0)
    {
        if(m_grounded)
        {
            if(m_wantsToJump)
            {
                m_velocity.y = 0.0125f;
            }
        }
        //FallMultiplier = 1.f;
    }
    //else FallMultiplier = 0.25f;

    m_velocity.y -= m_gravity*DeltaTime*FallMultiplier;
    
    const uint8_t Normals = GetParent()->GetCollider()->Move(m_velocity, &GetParent()->GetWorld()->getManager());
    m_grounded = (Normals & EntityC_Up) != 0;
    if(Normals & EntityC_Down) m_velocity.y = 0;

    m_inputVector = glm::vec3(0);
}

glm::vec3 CEntityMovement::Accelerate(float DeltaTime, const glm::vec3& accelDir, const glm::vec3& prevVelocity, float accelerate, float max_velocity)
{
    float projVel = glm::dot(prevVelocity, accelDir); // Vector projection of Current velocity onto accelDir.
    float accelVel = accelerate * DeltaTime; // Accelerated velocity in direction of movment

    // If necessary, truncate the accelerated velocity so the vector projection does not exceed max_velocity
    if(projVel + accelVel >= max_velocity)
        accelVel = max_velocity - projVel;

    return prevVelocity + accelDir * accelVel;
}

glm::vec3  CEntityMovement::MoveGround(float DeltaTime, const glm::vec3& accelDir, glm::vec3 prevVelocity)
{
    const float speed = glm::length(prevVelocity);
    if (speed > 0)
    {
        float drop = speed * m_friction * DeltaTime;
        prevVelocity *= std::max(speed - drop, 0.f) / speed;
    }
    return Accelerate(DeltaTime, accelDir, prevVelocity, m_acceletation, m_maxSpeed);
}

glm::vec3  CEntityMovement::MoveAir(float DeltaTime, const glm::vec3& accelDir, glm::vec3 prevVelocity)
{
    const float speed = glm::length(prevVelocity);
    if (speed > 0)
    {
        float drop = speed * m_airresistance * DeltaTime;
        prevVelocity *= std::max(speed - drop, 0.f) / speed;
    }
    return Accelerate(DeltaTime, accelDir, prevVelocity, m_airaccel, 10000.f);
}

CEntityMovement::CEntityMovement(CGameEntity* Parent) : CEntityComponent(Parent)
{
}

void CEntityMovement::AddMovementVector(const glm::vec3& InputVector)
{
    m_inputVector += InputVector;
}

void CEntityMovement::SetMaximumWalkingSpeed(float Speed)
{
    m_maxSpeed = Speed;
}

void CEntityMovement::SetWantsToJump(bool intention)
{
    m_wantsToJump = intention;
}