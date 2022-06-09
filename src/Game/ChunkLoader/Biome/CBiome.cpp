#include "CBiome.h"
#include <glm/gtc/noise.hpp>
#include <cmath>
#include "../CChunkGenerator.h"
#include <iostream>
#include <glm/gtx/compatibility.hpp>

// https://helloacm.com
inline float 
BilinearInterpolation(float botLeft, float topLeft, float botRight, float topRight, float xMin, float xMax, float yMin, float yMax, float x, float y) 
{
    float x2x1, y2y1, x2x, y2y, yy1, xx1;
    x2x1 = xMax - xMin;
    y2y1 = yMax - yMin;
    x2x = xMax - x;
    y2y = yMax - y;
    yy1 = y - yMin;
    xx1 = x - xMin;
    return 1.0 / (x2x1 * y2y1) * (
        botLeft * x2x * y2y +
        botRight * xx1 * y2y +
        topLeft * x2x * yy1 +
        topRight * xx1 * yy1
    );
}

float blerp(float c00, float c10, float c01, float c11, float tx, float ty){
    return glm::lerp(glm::lerp(c00, c10, tx), glm::lerp(c01, c11, tx), ty);
}

std::unique_ptr<SBlockInfo[]> CBiome::generateChunk(int cx, int cy, int cz)
{
	std::lock_guard m(m_destruction);

    std::unique_ptr<SBlockInfo[]> output = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
	StructureInfo info;

	int downPos = cy * CHUNK_SIZE; /*botton of the chunk*/
	int upPos = cy * CHUNK_SIZE+CHUNK_SIZE; /*top of the chunk*/
	int top = 0;
    const int transition_scale = CHUNK_SIZE*0.5f;

	static auto caveHere = [&](int x, int y, int z)
	{
		return simplexCave(x, y, z) < 0.25;
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
			/*float xMin, xMax=cx * CHUNK_SIZE + HALF_CHUNK*(1 + x/HALF_CHUNK), zMin, zMax=cz * CHUNK_SIZE + HALF_CHUNK*(1 + z/HALF_CHUNK);
			xMin=xMax-CHUNK_SIZE;
			zMin=zMin-CHUNK_SIZE;
			for(int tx = -1; tx <= 1; ++tx)
			{
				for(int tz = -1; tz <= 1; ++tz)
				{
					CBiome* biome = m_parent->getBiome(cx+tx, cz+tz);
					if(biome->m_id != m_id)
					{
						top = BilinearInterpolation(m_parent->getTop(xMin, zMax), 
													m_parent->getTop(xMin, zMin), 
													m_parent->getTop(xMax, zMax), 
													m_parent->getTop(xMax, zMin), xMin, xMax, zMin, zMax, gx, gz);
						break;
					}
				}
			}*/

            /*CBiome* NW = m_parent->getBiome(gx+transition_scale, gz-transition_scale);
            CBiome* NE = m_parent->getBiome(gx+transition_scale, gz+transition_scale);
            CBiome* SE = m_parent->getBiome(gx-transition_scale, gz+transition_scale);
            CBiome* SW = m_parent->getBiome(gx-transition_scale, gz-transition_scale);
            top = simplexPlains(gx, gz);

            //if(NW->m_id != m_id || NE->m_id != m_id || SE->m_id != m_id || SW->m_id != m_id)
            {
                int topNW = NW->simplexPlains(gx+transition_scale, gz-transition_scale), 
                    topNE = NE->simplexPlains(gx+transition_scale, gz+transition_scale),
                    topSE = SE->simplexPlains(gx+transition_scale, gz+transition_scale), 
                    topSW = SW->simplexPlains(gx+transition_scale, gz-transition_scale);

                float xmin = gx-transition_scale, xmax=gx+transition_scale;
                float zmin = gz-transition_scale, zmax=gz+transition_scale;

                if(xmin < xmax) std::swap(xmin, xmax);
                if(zmin < zmax) std::swap(zmin, zmax);

                float dx = (gx- xmin)/(xmax-xmin);
                float dz = (gz- zmin)/(zmax-zmin);

                top = BilinearInterpolation(topSW, topSE, topNW, topNE, xmin, xmax, zmin, zmax, gx, gz);
            }*/
			top = simplexPlains(gx, gz);

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
						output[getIndex(x, i, z)].id = m_topBlock;
						if(i+1<CHUNK_SIZE && output[getIndex(x, i+1, z)].id ==BLOCK_AIR)
						{
							srand(x^i^z);
							if((rand()&15)==0)
								output[getIndex(x, i+1, z)].id = BLOCK_PEBBLE;
						}
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
								output[getIndex(x, i, z)].id = m_underBlock;
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

float CBiome::simplexPlains(float x, float z) const
{
    return m_level+OctaveSimplex(x/ (float)(CHUNK_SIZE*5), 0, z/ (float)(CHUNK_SIZE*5), 8, 0.25)*m_intesity;
}
float CBiome::simplexCave(float x, float y, float z) const
{
    return OctaveSimplex(x / (float)(CHUNK_SIZE*5), y/(float)(std::max(CHUNK_SIZE*2.f,std::min(42.f, (CHUNK_SIZE*(std::abs(64-y)/30))))), z / (float)(CHUNK_SIZE * 5), 8, 0.25) ;
}

float CBiome::OctaveSimplex(float x, float y, float z, int octaves, float persistence) const
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

void CBiome::consumeCache(CChunkManager* world)
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

CBiome* CBiome::setTopBlock(int topBlock)
{
    m_topBlock = topBlock;
    return this;
}

CBiome* CBiome::setUnderBlock(int underBlock)
{
    m_underBlock = underBlock;
    return this;
}

CBiome* CBiome::addStructure(CStructure* structure)
{
    m_structures.push_back(std::unique_ptr<CStructure>(structure));
    return this;
}

CBiome* CBiome::setWaterLevel(int waterLevel)
{
    m_waterLevel = waterLevel;
    return this;
}

CBiome* CBiome::setLevel(int level)
{
    m_level = level;
    return this;
}

CBiome* CBiome::setIntensity(int intensity)
{
    m_intesity = intensity;
    return this;
}

CBiome::CBiome(CChunkGenerator* parent, int id) : m_parent(parent), m_id(id)
{
}

CBiome::~CBiome()
{
	std::lock_guard m(m_destruction);
	m_structures.clear();
}