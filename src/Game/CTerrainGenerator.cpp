#include "CTerrainGenerator.h"
#include "CChunk.h"
#include "CChunkPart.h"
#include "CBlock.h"
#include "Game/GameWorld/CWorld.h"

#include <glm/gtc/noise.hpp>
#include <iostream>


bool CTerrainGenerator::NeedTree(int x, int z)
{
	//return glm::simplex(glm::vec3(x+ CHUNK_SIZE*z, z + CHUNK_SIZE * x, 0)) > 0.89;
	srand(x^z);

	return (rand()%50 == 0);
}

float CTerrainGenerator::OctaveSimplex(float x, float y, float z, int octaves, float persistence)
{
	float total = 0.f;
	float frequency = 1.f;
	float amplitude = 1.f;
	float maxValue = 0.f;

	for (int i = 0; i < octaves; i++)
	{
		total += glm::simplex(glm::vec3(x * frequency, y * frequency, z * frequency)) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2;
	}

	return total;
}
//DEPRECATED
void CTerrainGenerator::makeChunk(int cx, int cy, int cz, CChunk* chunk)
{
	/*int downPos = cy * CHUNK_SIZE; //botton of the chunk
	int upPos = cy * CHUNK_SIZE+CHUNK_SIZE; //top of the chunk
	int top = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			int gx = cx * CHUNK_SIZE + x;//global coord x
			int gz = cz * CHUNK_SIZE + z;//global coord z

			/////////////FIND Y POS ON HEIGHTMAN//////////////////
			auto pos = std::make_pair(gx, gz);
			auto it = m_heightMap.find(pos);

			if (it == m_heightMap.end())
				m_heightMap.emplace(pos, 64 + OctaveSimplex(gx / (float)(CHUNK_SIZE*5), 0, gz / (float)(CHUNK_SIZE * 5), 8, 0.25) * 16);
			top = m_heightMap[pos];

			m_maxy = std::max(m_maxy, top / CHUNK_SIZE); //set max chunk pos


			if (downPos <= top && top < upPos)
			{
				int max = top - downPos;
				for (int i = 0; i < max; i++)
					chunk->SetBlock(x, i, z, (top-5 < downPos+i) ? BLOCK_DIRT : BLOCK_STONE);
				chunk->SetBlock(x, max, z, BLOCK_GRASS);

				if (m_tree.shouldBeHere(gx, gz))
					m_tree.getInfo(gx, top, gz, m_cache);
			}
			else if(downPos < top)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					//int gy = downPos + i;
					chunk->SetBlock(x, i, z, BLOCK_STONE);
				}
			}
		}
	}*/
}

void CTerrainGenerator::makeChunkPart(int cx, int cz, CChunkPart* part)
{
	for (int y = 0; y <= m_maxy; y++)
		makeChunk(cx, y, cz, part->getChunkNew(y));
}

void CTerrainGenerator::generateWorld(std::tuple<int, int, int> playerPos, int distance, CChunkManager* world)
{
	int cx = std::get<0>(playerPos);
	//int cy = std::get<1>(playerPos);
	int cz = std::get<2>(playerPos);
	distance /= 2;
	distance += 1; /*Lazy chunk*/
	m_heightMap.clear();
	for(int x = cx - distance; x <= cx + distance; x++) 
	for(int z = cz - distance; z <= cz + distance; z++)
	{
		bool existed;
		CChunkPart* part = world->createChunkIfNone(x, z, &existed);
		if (existed)
			continue;
		makeChunkPart(x, z, part);
	}



	consumeCache(world);
	/*for (uint32_t i = 0; i < m_cache.size(); i++)
	{
		CChunkPart* part = world->getChunkPart(std::get<0>(m_cache[i].first), std::get<2>(m_cache[i].first));
		//std::cout << std::get<0>(m_cache[i].first) << ' ' << std::get<1>(m_cache[i].first) << ' ' << std::get<2>(m_cache[i].first) << '\n';
		CChunk* chunk = 0x0;
		if (part)
			chunk = part->getChunk(std::get<1>(m_cache[i].first));
		if (!chunk) continue;

		for (uint32_t j = 0; j < m_cache[i].second.size(); j++)
		{
			chunk->setChunkGenerating(true);
			chunk->SetBlock(std::get<0>(m_cache[i].second[j].first), std::get<1>(m_cache[i].second[j].first), std::get<2>(m_cache[i].second[j].first), m_cache[i].second[j].second);
			chunk->setChunkGenerating(false);
		}
		m_cache.erase(m_cache.begin()+i);
	}*/


}

void CTerrainGenerator::consumeCache(CChunkManager* world)
{
	if (m_cache.empty()) return;

	for (auto it = m_cache.begin(); it != m_cache.end();)
	{
		CChunkPart* part = world->getChunkPart(std::get<0>(it->first), std::get<2>(it->first));
		if (!part)
		{
			++it;
			continue;
		}
		CChunk *chunk = part->getChunkNew(std::get<1>(it->first));

		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			chunk->getVoxelComponent().setBlock(std::get<0>(it2->first), std::get<1>(it2->first), std::get<2>(it2->first), it2->second);
		it = m_cache.erase(it);
	}
}

std::vector<glm::ivec4> CTerrainGenerator::getTree(int i)
{
	std::vector<glm::ivec4> output;

	switch (i)
	{
	case 0:
		output =
		{
			{0, 1, 0, BLOCK_LOG},
			{0, 2, 0, BLOCK_LOG},
			{0, 3, 0, BLOCK_LOG},
			{0, 4, 0, BLOCK_LOG}
		};
		for (int i = -3; i <= 3; i++)
		for (int j = -3; j <= 3; j++)
				output.push_back({ i, 4, j, BLOCK_LEAVES });

		for (int i = -3; i <= 3; i++)
		for (int j = -3; j <= 3; j++)
			output.push_back({ i, 5, j, BLOCK_LEAVES });

		for (int i = -2; i <= 2; i++)
		for (int j = -2; j <= 2; j++)
			output.push_back({ i, 6, j, BLOCK_LEAVES });

		for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			output.push_back({ i, 7, j, BLOCK_LEAVES });

		break;
	}

	return output;
}

