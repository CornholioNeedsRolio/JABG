#include "CChunkCollider.h"
#include <vector>
#include <iostream>
#include <algorithm>

void CChunkCollider::Divide()
{
	if (m_octree)
		return;

	glm::vec3 halfs = m_size*0.5f; 

	if (halfs.x < 1)
		return;

	m_octree = std::unique_ptr<CChunkCollider[]>(new CChunkCollider[8]
		{
			CChunkCollider(m_pos									, halfs), //NW
			CChunkCollider(m_pos + glm::vec3(halfs.x, 0, 0)			, halfs), //NE
			CChunkCollider(m_pos + glm::vec3(0, 0, halfs.z)			, halfs), //SW
			CChunkCollider(m_pos + glm::vec3(halfs.x, 0, halfs.z)	, halfs), //SE
								  
			CChunkCollider(m_pos + glm::vec3(0,halfs.y,0)					, halfs), //NWU
			CChunkCollider(m_pos + glm::vec3(halfs.x, halfs.y, 0)			, halfs), //NEU
			CChunkCollider(m_pos + glm::vec3(0, halfs.y, halfs.z)			, halfs), //SWU
			CChunkCollider(m_pos + glm::vec3(halfs.x, halfs.y, halfs.z)		, halfs), //SEU
		});

	for (int i = 0; i < 8; ++i)
		m_octree[i].setInfo(m_info);
}

CChunkCollider::CChunkCollider(glm::vec3 position, glm::vec3 size) :
	m_pos(position), m_size(size), m_collider(m_pos, m_size+m_pos)
{
	m_info = BLOCKCOLLIDER_NONE;
}

CChunkCollider::~CChunkCollider()
{
	
}

void CChunkCollider::Draw(const SDrawInfo& info)
{
	if (!m_octree)
	{
		if (m_info != BLOCKCOLLIDER_NONE)
			m_collider.Draw(info);
	}
	else
	{
		for (int i = 0; i < 8; ++i)
			m_octree[i].Draw(info);
	}
	
}

void CChunkCollider::setInfo(int x, int y, int z, ColliderType type)
{
	if (intersectsPoint(x, y, z))
	{
		Divide();
		if (!m_octree)
			m_info = type;
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				if (m_octree[i].intersectsPoint(x, y, z))
				{
					m_octree[i].setInfo(x, y, z, type);

					bool shouldMerge = true;
					for (int i = 0; i < 8; ++i)
						shouldMerge &= m_octree[i].shouldMerge(type);
					if (shouldMerge)
					{
						m_octree = 0x0;
						m_info = type;
					}
					return;
				}
			}
		}
	}
}

void CChunkCollider::setInfo(ColliderType x)
{
	m_info = x;
}

bool CChunkCollider::shouldMerge(ColliderType type)
{
	return !m_octree && m_info == type;
}

bool CChunkCollider::intersectsPoint(float x, float y, float z)
{
	return m_collider.collidesWithPoint(glm::vec3(x,y,z)+glm::vec3(0.5));
}

bool CChunkCollider::applyCollisionWithGEC(CAABB& other, glm::vec3& velocity, std::vector<std::pair<CAABB*, float>>& collisions)
{

	glm::vec3 normal; float delta;

	if (!m_octree)
	{
		if (m_info == BLOCKCOLLIDER_FULL)
			if (other.RaySweep(m_collider, velocity, normal, delta))
			{
				collisions.push_back(std::make_pair(&m_collider, delta));
				return true;
			}
		return false;
	}
	else
	{
		bool inside = true;
		if (other.RaySweep(m_collider, velocity, normal, delta, &inside) || inside)
		{
			bool output = false;
				for (int i = 0; i != 8; i++)
					output |= m_octree[i].applyCollisionWithGEC(other, velocity, collisions);
				return output;
		}
		return false;
	}
}

bool CChunkCollider::RayCastWithGEC(const glm::vec3& position, const glm::vec3& offset, std::vector<std::pair<glm::vec3, float>>& collisions)
{
	glm::vec3 normal; float delta;

	if (!m_octree)
	{
		if (m_info == BLOCKCOLLIDER_FULL)
			if (m_collider.RayCast(position, offset, normal, delta))
			{
				if(delta <= 1 && delta >= 0)
				{
					collisions.push_back(std::make_pair(normal, delta));
					return true;
				}
			}
		return false;
	}
	else
	{
		bool inside;
		if(m_collider.RayCast(position, offset, normal, delta, &inside) || inside)
		{
			bool output = false;
			for (int i = 0; i != 8; i++)
				output |= m_octree[i].RayCastWithGEC(position, offset, collisions);
			return output;
		}
		return false;
	}
}

const ColliderType& CChunkCollider::getType(bool& divided)
{
	if (!m_octree)
		divided = false;
	else 
		divided = true;

	return m_info;
}

bool CChunkCollider::Fractured()
{
	return m_octree != 0x0;
}

CAABB& CChunkCollider::getCollider()
{
	return m_collider;
}
