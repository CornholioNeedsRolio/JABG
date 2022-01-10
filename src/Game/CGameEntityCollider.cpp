#include "CGameEntityCollider.h"
#include "CChunkManager.h"
#include "CChunkCollider.h"
#include <iostream>
#include <algorithm>

CGameEntityCollider::CGameEntityCollider(const glm::vec3& min, const glm::vec3& max) : 
	m_collider(min, max)
{

}

uint8_t CGameEntityCollider::Move(glm::vec3& velocity, CChunkManager* manager)
{
	if (velocity == glm::vec3(0)) return 0;

	bool collision = false;
	uint8_t output = 0;
	std::vector<std::pair<CAABB*, float>> collisions = {};

	for (auto it = manager->getBeginIterator(); it != manager->getEndIterator(); it++)
		for (auto chunk = it->second.getBeginIterator(); chunk != it->second.getEndIterator(); chunk++)
		{
			if (chunk->second->intersectsCollider(&m_collider, velocity))
				chunk->second->generateCollider();
			else
				continue;

			CChunkCollider* collider = chunk->second->getCollider();
			
			collision |= collider->applyCollisionWithGEC(m_collider, velocity, collisions);

		}

	//std::cout << collision << '\n';
	if (collision)
	{
		/*
			*Here lies a hidden bug
			*it's a bug in AABB class
			*normals are a headache
		*/

		std::sort(collisions.begin(), collisions.end(), [&](const std::pair<CAABB*, float>& a, const std::pair<CAABB*, float>& b) {if (a.second == b.second) { return glm::length(m_collider.getCenterVector() - a.first->getCenterVector()) < glm::length(m_collider.getCenterVector() - b.first->getCenterVector()); }  return a.second < b.second; });
		for (int i = 0; i < collisions.size(); ++i) 
		{
			glm::vec3 normal(0);
			glm::vec3 tempvel(0);
			glm::vec3 tempvel1(0);

			m_collider.ResolveDynamicSweep(*collisions[i].first, velocity, normal);
			if (normal.x > 0) output |= EntityC_Right;
			if (normal.x < 0) output |= EntityC_Left;

			if (normal.y > 0) output |= EntityC_Up;
			if (normal.y < 0) output |= EntityC_Down;

			if (normal.z > 0) output |= EntityC_Front;
			if (normal.z < 0) output |= EntityC_Back;

		}
	}

	m_collider.Move(velocity);
	return output;
	//std::cout << velocity.x << ' ' << velocity.y << ' ' << velocity.z << '\n';



}

bool CGameEntityCollider::RayCast(const glm::vec3& position, const glm::vec3& offset, glm::vec3& worldpos, glm::vec3& normal, CChunkManager* manager)
{
	bool collision = false;
	//uint8_t output = 0;
	std::vector<std::pair<glm::vec3, float>> collisions = {};

	for (auto it = manager->getBeginIterator(); it != manager->getEndIterator(); it++)
		for (auto chunk = it->second.getBeginIterator(); chunk != it->second.getEndIterator(); chunk++)
		{
			if (chunk->second->intersectsCollider(&m_collider, offset))
				chunk->second->generateCollider();
			else
				continue;

			CChunkCollider* collider = chunk->second->getCollider();

			collision |= collider->RayCastWithGEC(position, offset, collisions);

		}

	//std::cout << collision << '\n';
	if (collision)
	{
		std::sort(collisions.begin(), collisions.end(), [](const std::pair<glm::vec3, float>& a, const std::pair < glm::vec3 , float > & b) {return a.second < b.second; });
		worldpos = position + offset*collisions[0].second;
		normal = collisions[0].first;
		return true;
	}
	return false;
}

void CGameEntityCollider::forceRegen(glm::ivec3 position, class CChunkManager* manager)
{
	for (auto it = manager->getBeginIterator(); it != manager->getEndIterator(); it++)
		for (auto chunk = it->second.getBeginIterator(); chunk != it->second.getEndIterator(); chunk++)
		{
			if (chunk->second->getChunkPosition() == position)
			{
				chunk->second->generateCollider();
				return;
			}
		}
}

CAABB& CGameEntityCollider::getCollider()
{
	return m_collider;
}
