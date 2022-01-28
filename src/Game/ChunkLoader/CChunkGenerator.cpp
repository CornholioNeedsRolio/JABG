#include "CChunkGenerator.h"
#include <glm/gtc/noise.hpp>
#include <glm/glm.hpp>
#include "../CWorld.h"


std::unique_ptr<SBlockInfo[]> CChunkGenerator::generateChunk(int cx, int cy, int cz)
{
	std::lock_guard m(m_destruction);

    std::unique_ptr<SBlockInfo[]> output = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
	StructureInfo info;

	int downPos = cy * CHUNK_SIZE; /*botton of the chunk*/
	int upPos = cy * CHUNK_SIZE+CHUNK_SIZE; /*top of the chunk*/
	int top = 0;
	static auto caveHere = [&](int x, int y, int z)
	{
		return OctaveSimplex(x / (float)(CHUNK_SIZE*5), y/(float)(std::max(CHUNK_SIZE*2,std::min(42, (CHUNK_SIZE*(std::abs(64-y)/30))))), z / (float)(CHUNK_SIZE * 5), 8, 0.25) < 0.25;
	};

	static auto getIndex = [](int x, int y, int z)
	{
		return x + CHUNK_SIZE*(y + z * CHUNK_SIZE);
	};

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			int gx = cx * CHUNK_SIZE + x;//global coord x
			int gz = cz * CHUNK_SIZE + z;//global coord z

			/////////////FIND Y POS ON HEIGHTMAP//////////////////
			top = 64+OctaveSimplex(gx / (float)(CHUNK_SIZE*5), 0, gz / (float)(CHUNK_SIZE * 5), 8, 0.25) * 16;//m_heightMap[pos];
			m_max = std::max(m_max, top / CHUNK_SIZE+1);
			if (downPos <= top && top < upPos)
			{
				int max = top - downPos;
				for (int i = 0; i < max; i++)
                    output[getIndex(x, i, z)].id = caveHere(gx, cy*CHUNK_SIZE+i, gz) ? BLOCK_STONE : BLOCK_AIR;
			}
			else if(downPos < top)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
                    output[getIndex(x, i, z)].id = caveHere(gx, cy*CHUNK_SIZE+i, gz) ? BLOCK_STONE : BLOCK_AIR;
			}

			for(int i = CHUNK_SIZE-1; i >= 0; --i)
			{
				int current = output[getIndex(x, i, z)].id;
				if(current == BLOCK_STONE)
				{
					int gy = cy*CHUNK_SIZE + i;
					int distTop = std::abs(top - gy);
					bool aboveBlockMissing = distTop <= 1 || (!caveHere(gx, gy+1, gz) && distTop <= 30);

					if(aboveBlockMissing)
					{
						output[getIndex(x, i, z)].id = BLOCK_GRASS;
						for(int c = 0; c < m_structures.size(); c++)
						{
							if(m_structures[c]->shouldBeHere(gx, gy+1, gz) && m_structures[c]->getStructureType() == Structure_Tree)
							{
								m_structures[c]->getInfo(gx, gy+1, gz, info);
							}
						}
					}
					else
					{
						for(int j = 0; j <= 5; ++j)
						{
							int _gy = gy+j;
							int _distTop = std::abs(top - _gy);
							bool aboveBlockMissing = _distTop <= 1 || (!caveHere(gx, _gy+1, gz) && _distTop <= 30);
							if(aboveBlockMissing)
							{
								output[getIndex(x, i, z)].id = BLOCK_DIRT;
								break;
							}
						}
					}
				}
			}
		}
	}

	for(int i = 0; i != info.size(); ++i)
		if(std::get<0>(info[i].first) == cx && std::get<1>(info[i].first) == cy && std::get<2>(info[i].first) == cz)
		{
			for(int j = 0; j < info[i].second.size(); ++j)
			{
				int index = getIndex(std::get<0>(info[i].second[j].first), std::get<1>(info[i].second[j].first), std::get<2>(info[i].second[j].first));
				if(output[index].id == BLOCK_AIR)
					output[index].id = info[i].second[j].second;
			}

			info.erase(info.begin()+i);
			break;
		}
	m_mutex.lock();
	m_structInfo.insert(m_structInfo.end(), info.begin(), info.end());
	m_mutex.unlock();
    return output;
}

int CChunkGenerator::getMax() const
{
	return m_max;
}

std::shared_ptr<class CChunk> CChunkGenerator::getChunk(int x, int y, int z, CWorld* world)
{
    std::shared_ptr<CChunk> chunk = std::make_shared<CChunk>(glm::ivec3(x,y,z));
	if(!chunk->getSaveComponent().load(world->getFilePath()))
    	chunk->getVoxelComponent().setAllBlocks(generateChunk(x, y, z));
    return chunk;
}

float CChunkGenerator::OctaveSimplex(float x, float y, float z, int octaves, float persistence)
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

void CChunkGenerator::consumeCache(CChunkManager* world)
{
	std::lock_guard m(m_mutex);
	if(m_structInfo.empty())
		return;
	
	for (auto it = m_structInfo.begin(); it != m_structInfo.end();)
	{
		CChunkPart* part = world->getChunkPart(std::get<0>(it->first), std::get<2>(it->first));
		if (!part)
		{
			++it;
			continue;
		}
		CChunk *chunk = part->getChunk(std::get<1>(it->first));
		if(!chunk) 
		{
			++it;
			continue;
		}
		chunk->getVoxelComponent().setBlocksInBulk(it->second);
		it = m_structInfo.erase(it);
	}
}

CChunkGenerator::CChunkGenerator()
{
	m_structures.push_back(std::make_unique<CStructureTree>());
}

CChunkGenerator::~CChunkGenerator()
{
	std::lock_guard m(m_destruction);
	m_structures.clear();
}