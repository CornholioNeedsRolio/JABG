#include "CBiome.h"
#include <glm/gtc/noise.hpp>
#include <cmath>
#include "../CChunkGenerator.h"
#include "CBiomeMap.h"
#include <iostream>
#include <glm/gtx/compatibility.hpp>

// https://helloacm.com

float CBiome::simplexPlains(float x, float z) const
{
    return getTopBlock()+OctaveSimplex(x/ (float)(CHUNK_SIZE*5), 0, z/ (float)(CHUNK_SIZE*5), 8, 0.25)*m_intesity;
}
float CBiome::simplexCave(float x, float y, float z) const
{
    return OctaveSimplex(x / (float)(CHUNK_SIZE*5), y/(float)(std::max(CHUNK_SIZE*2.f,std::min(42.f, (CHUNK_SIZE*(std::abs(64-y)/30))))), z / (float)(CHUNK_SIZE * 5), 8, 0.25) ;
}

bool CBiome::caveHere(int x, int y, int z) const
{
	return simplexCave(x, y, z) < m_minForCave;
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
void CBiome::AddStructure(int gx, int gy, int gz, StructureInfo& info, std::mutex& mutex)
{
	for(int i = 0; i < m_structures.size(); ++i)
	{
		if(m_structures[i]->shouldBeHere(gx, gy+1, gz))
		{
			mutex.lock();
			m_structures[i]->getInfo(gx, gy+1, gz, info);
			mutex.unlock();
		}
	}
}

CBiome* CBiome::setMinCave(float minCave)
{
	m_minForCave = minCave;
	return this;
}

int CBiome::getVegetationBlock(int x, int y, int z) const
{
	if(m_vegetations.size() == 0) return 0;
	if(x) srand((int)(x+y*z/x));
	else srand(69);
	return m_vegetations[rand()%m_vegetations.size()];
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

 CBiome* CBiome::addVegetation(int BlockID)
 {
	m_vegetations.push_back(BlockID);
	return this;
 }

CBiome* CBiome::setIntensity(int intensity)
{
    m_intesity = intensity;
    return this;
}

int CBiome::getTopBlock() const
{
	return m_topBlock;
}

int CBiome::getUnderBlock() const
{
	return m_underBlock;
}

int CBiome::getWaterLevel() const
{
	return m_waterLevel;
}

int CBiome::getLevel() const
{
	return m_level;
}

int CBiome::getIntensity() const
{
	return m_intesity;
}


CBiome::CBiome(CChunkGenerator* parent, int id) : m_parent(parent), m_id(id)
{
}

CBiome::~CBiome()
{
	m_structures.clear();
}