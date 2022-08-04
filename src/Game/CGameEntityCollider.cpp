#include "CGameEntityCollider.h"
#include "Game/ChunkManager/CChunkManager.h"
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
	uint8_t output = 0;

	glm::vec3 min = m_collider.getMinVector();
	glm::vec3 max = m_collider.getMaxVector();
	std::vector<glm::ivec3> NormalZero{};
	for(int x = min.x-1; x <= max.x+1; ++x)
	{
		for(int y = min.y-1; y <= max.y+1; ++y)
		{
			for(int z = min.z-1; z <= max.z+1; ++z)
			{
				std::vector<glm::ivec3> blocks = RayCastBlocks({x,y,z}, velocity);
				for(int i = 0; i < blocks.size(); ++i)
				{
					bool collision = false;
					auto pos = manager->getLocal(blocks[i].x, blocks[i].y, blocks[i].z);
					CChunkPart* part = manager->getChunkPart(std::get<0>(pos.first), std::get<2>(pos.first)); CChunk* chunk = nullptr;
					if(part) chunk = part->getChunk(std::get<1>(pos.first));
					if(!chunk || !part)
						collision = true;
					else
					{
						CBlock* block = chunk->getVoxelComponent().getBlock(std::get<0>(pos.second), std::get<1>(pos.second), std::get<2>(pos.second)).getBlock();
						if(block->getCollision() == BLOCKCOLLIDER_FULL && block->isSolid())
							collision = true;
					}

					if(collision)
					{
						CAABB normalBox(glm::vec3{blocks[i].x, blocks[i].y, blocks[i].z}, glm::vec3{blocks[i].x+1, blocks[i].y+1, blocks[i].z+1});
						glm::vec3 normal(0);
						if(m_collider.ResolveDynamicSweep(normalBox, velocity, normal))
						{
							if(normal.x == 0 && normal.y == 0 && normal.z == 0)
							{
								NormalZero.push_back(blocks[i]);
								continue;
							}

							if (normal.x > 0) output |= EntityC_Right;
							else if (normal.x < 0) output |= EntityC_Left;

							else if (normal.y > 0) output |= EntityC_Up;
							else if (normal.y < 0) output |= EntityC_Down;

							else if (normal.z > 0) output |= EntityC_Front;
							else if (normal.z < 0) output |= EntityC_Back;
						}
					}
				}
			}
		}
	}
	if(!(output & EntityC_Up))
	{
		for(int i = 0; i < NormalZero.size(); ++i)
		{
			CAABB normalBox(glm::vec3{NormalZero[i].x, NormalZero[i].y, NormalZero[i].z}, glm::vec3{NormalZero[i].x+1, NormalZero[i].y+1, NormalZero[i].z+1});
			glm::vec3 normal(0);
			m_collider.ResolveDynamicSweep(normalBox, velocity, normal, true);
		}
	}
	
	m_collider.Move(velocity);

	return output;
}

//https://lodev.org/cgtutor/raycasting.html#Untextured_Raycaster_
bool CGameEntityCollider::RayCast(const glm::vec3& position, const glm::vec3& offset, glm::vec3& worldpos, glm::vec3& normal, CChunkManager* manager, float* deltaDir)
{
	glm::ivec3 mapPos = glm::floor(position);
	glm::vec3 direction = glm::normalize(offset);
	glm::vec3 delta; for(int i = 0; i < 3; ++i) delta[i] = direction[i] == 0 ? 10e20 : std::abs(1.f / direction[i]);
	const float maxLenght = glm::length(offset);
	normal *= 0;
	glm::ivec3 step;
	glm::vec3 distance;
	if(direction.x < 0)
	{
		step.x = -1;
		distance.x = (position.x - mapPos.x)*delta.x;
	}
	else
	{
		step.x = 1;
		distance.x = (mapPos.x + 1 - position.x)*delta.x;
	}

	if(direction.y < 0)
	{
		step.y = -1;
		distance.y = (position.y - mapPos.y)*delta.y;
	}
	else
	{
		step.y = 1;
		distance.y = (mapPos.y + 1 - position.y)*delta.y;
	}

	if(direction.z < 0)
	{
		step.z = -1;
		distance.z = (position.z - mapPos.z)*delta.z;
	}
	else
	{
		step.z = 1;
		distance.z = (mapPos.z + 1 - position.z)*delta.z;
	}

	float lenght = 0;
	bool stop = false;
	int failTicks = 0;
	while(!stop && lenght < maxLenght && failTicks++ < 100)
	{
		//todo make this more optimal
		auto pos = manager->getLocal(mapPos.x, mapPos.y, mapPos.z);
		CChunkPart* part = manager->getChunkPart(std::get<0>(pos.first), std::get<2>(pos.first)); CChunk* chunk = nullptr;
		if(part) chunk = part->getChunk(std::get<1>(pos.first));
		if(!chunk || !part)
		{
			stop = true;
			break;
		}
		else
		{
			CBlock* block = chunk->getVoxelComponent().getBlock(std::get<0>(pos.second), std::get<1>(pos.second), std::get<2>(pos.second)).getBlock();
			if(block->getCollision() == BLOCKCOLLIDER_FULL && block->isTargetable())
			{
				stop = true;
				break;
			}
		}

		if(distance.x < distance.y && distance.x < distance.z)
		{
			mapPos.x += step.x;
			lenght = distance.x;
			distance.x += delta.x;
			normal = glm::vec3(-step.x, 0, 0);
		}
		else if(distance.y < distance.z)
		{
			mapPos.y += step.y;
			lenght = distance.y;
			distance.y += delta.y;
			normal = glm::vec3(0, -step.y, 0);
		}
		else
		{
			mapPos.z += step.z;
			lenght = distance.z;
			distance.z += delta.z;
			normal = glm::vec3(0, 0, -step.z);
		}
	}
	if(stop)
	{
		worldpos = position + direction*lenght;
		if(deltaDir)
			*deltaDir = lenght/maxLenght;
		return true;
	}

	return false;
	/*bool collision = false;
	std::vector<std::pair<glm::vec3, float>> collisions = {};
	return false;
	
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
	
	if (collision)
	{
		std::sort(collisions.begin(), collisions.end(), [](const std::pair<glm::vec3, float>& a, const std::pair < glm::vec3 , float > & b) {return a.second < b.second; });
		worldpos = position + offset*collisions[0].second;
		normal = collisions[0].first;
		return true;
	}
	return false;*/
}

std::vector<glm::ivec3> CGameEntityCollider::RayCastBlocks(const glm::vec3& position, const glm::vec3& offset)
{
	std::vector<glm::ivec3> output = {};
	glm::ivec3 mapPos = glm::floor(position);
	glm::vec3 direction = glm::normalize(offset);
	glm::vec3 delta; for(int i = 0; i < 3; ++i) delta[i] = direction[i] == 0 ? 10e20 : std::abs(1.f / direction[i]);
	const float maxLenght = glm::length(offset);
	glm::ivec3 step;
	glm::vec3 distance;
	if(direction.x < 0)
	{
		step.x = -1;
		distance.x = (position.x - mapPos.x)*delta.x;
	}
	else
	{
		step.x = 1;
		distance.x = (mapPos.x + 1 - position.x)*delta.x;
	}

	if(direction.y < 0)
	{
		step.y = -1;
		distance.y = (position.y - mapPos.y)*delta.y;
	}
	else
	{
		step.y = 1;
		distance.y = (mapPos.y + 1 - position.y)*delta.y;
	}

	if(direction.z < 0)
	{
		step.z = -1;
		distance.z = (position.z - mapPos.z)*delta.z;
	}
	else
	{
		step.z = 1;
		distance.z = (mapPos.z + 1 - position.z)*delta.z;
	}

	float lenght = 0;
	bool stop = false;
	while(!stop && lenght < maxLenght)
	{
		output.push_back(mapPos);
		if(distance.x < distance.y && distance.x < distance.z)
		{
			mapPos.x += step.x;
			lenght = distance.x;
			distance.x += delta.x;
		}
		else if(distance.y < distance.z)
		{
			mapPos.y += step.y;
			lenght = distance.y;
			distance.y += delta.y;
		}
		else
		{
			mapPos.z += step.z;
			lenght = distance.z;
			distance.z += delta.z;
		}
	}
	return output;
}

void CGameEntityCollider::forceRegen(glm::ivec3 position, class CChunkManager* manager)
{
	/*for (auto it = manager->getBeginIterator(); it != manager->getEndIterator(); it++)
		for (auto chunk = it->second.getBeginIterator(); chunk != it->second.getEndIterator(); chunk++)
		{
			if (chunk->second->getChunkPosition() == position)
			{
				chunk->second->generateCollider();
				return;
			}
		}*/
}

CAABB& CGameEntityCollider::getCollider()
{
	return m_collider;
}

/*uint8_t CGameEntityCollider::Move(glm::vec3& velocity, CChunkManager* manager)
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

	if (collision)
	{
		
		//	*Here lies a hidden bug
		//	*it's a bug in AABB class
		//	*normals are a headache
		

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
}*/