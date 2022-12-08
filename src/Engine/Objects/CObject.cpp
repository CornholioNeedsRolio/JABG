#include "CObject.h"
#include "Engine/GLRenderer/CCamera.h"
#include <glm/gtc/matrix_transform.hpp>

CObject::CObject(SVector3 _position, SVector3 _rotation) : 
	m_positon(_position), m_rotation(_rotation), m_size(1)
{
}

void CObject::Attach(CObject* _attachment)
{
	m_attachment = _attachment;
}

void CObject::Detach()
{
	m_attachment = nullptr;
}

glm::vec3 CObject::GetForwardVector()
{
	glm::mat4 matrix = getModelMatrix(nullptr);

	return -glm::normalize(matrix[2]);
}

glm::vec3 CObject::GetRightVector()
{
	glm::mat4 matrix = getModelMatrix(nullptr);

	return glm::normalize(matrix[0]);
}

const SVector3& CObject::GetPosition() const
{
	return m_positon;
}

void CObject::SetPosition(const SVector3& a)
{
	m_positon = a;
}

SVector3 CObject::GetRotation() const
{
	return m_rotation;
}

void CObject::SetRotation(const SVector3& a)
{
	m_rotation = a;
}

SVector3 CObject::GetGlobalPosition() const
{
	if (m_attachment)
		return m_positon + m_attachment->GetGlobalPosition();
	return m_positon;
}

void CObject::SetGlobalPosition(const SVector3& a)
{
	if(m_attachment)
		m_positon = a - m_attachment->GetGlobalPosition();
	else
	m_positon = a;
}

SVector3 CObject::GetGlobalRotation() const
{
	if (m_attachment)
		return m_rotation + m_attachment->GetGlobalRotation();
	return m_rotation;
}

void CObject::SetGlobalRotation(const SVector3& a)
{
	if (m_attachment)
		m_rotation = a - m_attachment->GetGlobalRotation();
	else
	m_rotation = a;
}

const SVector3& CObject::getSize() const
{
	return m_size;
}

void CObject::setSize(const SVector3& size)
{
	m_size = size;
}

void CObject::Move(glm::vec3 velocity)
{
	m_positon += velocity;
}

void CObject::setIgnoreCamera(bool value)
{
	m_ignoreCamera = value;
}

SMat4 CObject::getModelMatrix(CCamera* camera)
{
	glm::vec3 position = GetGlobalPosition();
	if(camera && !m_ignoreCamera)
		position -= camera->GetGlobalPosition();
	glm::mat4 transform = glm::translate(glm::mat4(1), position);
	glm::mat4 rotation = glm::mat4(1);
	rotation *= glm::rotate(glm::mat4(1), glm::radians(GetGlobalRotation().x), { 1, 0, 0 }); //pitch
	rotation *= glm::rotate(glm::mat4(1), glm::radians(GetGlobalRotation().y), { 0, 1, 0 }); //yaw
	rotation *= glm::rotate(glm::mat4(1), glm::radians(GetGlobalRotation().z), { 0, 0, 1 }); //roll

	return transform * rotation * glm::scale(glm::mat4(1), m_size);
}
