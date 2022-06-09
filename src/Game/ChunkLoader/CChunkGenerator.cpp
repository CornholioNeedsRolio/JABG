#include "CChunkGenerator.h"
#include <glm/gtc/noise.hpp>
#include "Biome/CBiome.h"
#include <glm/glm.hpp>
#include "../CWorld.h"
#include <iostream>
#include <csignal>

std::shared_ptr<class CChunk> CChunkGenerator::getChunk(int x, int y, int z, CWorld* world)
{
    std::shared_ptr<CChunk> chunk = std::make_shared<CChunk>(glm::ivec3(x,y,z));
	//if(!chunk->getSaveComponent().load(world->getFilePath()))
	{
		int biome = m_map.getBiome(x, z);
    	chunk->getVoxelComponent().setAllBlocks(m_biomes[biome]->generateChunk(x, y, z));
	}
    return chunk;
}

void CChunkGenerator::consumeCache(CChunkManager* world)
{
	for(int i = 0; i < TOTAL_BIOMES; ++i)
	{
		m_biomes[i]->consumeCache(world);
	}
}

CChunkGenerator::CChunkGenerator() : m_map(0, TOTAL_BIOMES)
{
	m_biomes[BIOME_PLAINS] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_PLAINS))->addStructure(new CStructureTree()));
	m_biomes[BIOME_DESERT] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_DESERT))->setTopBlock(BLOCK_SAND)->setUnderBlock(BLOCK_SAND));
	m_biomes[BIOME_OCEAN] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_OCEAN))->setTopBlock(BLOCK_SAND)->setUnderBlock(BLOCK_SAND)->setLevel(16)->setIntensity(30));
}

int CChunkGenerator::getTop(int x, int z) const
{
	int cx = std::floor((float)x / (float)CHUNK_SIZE);
	int cz = std::floor((float)z / (float)CHUNK_SIZE);
	return m_biomes[m_map.getBiome(cx, cz)]->simplexPlains(x, z);
}

CBiome* CChunkGenerator::getBiome(int cx, int cz) const
{
	return m_biomes[m_map.getBiome(cx, cz)].get();
}

CChunkGenerator::~CChunkGenerator()
{
}