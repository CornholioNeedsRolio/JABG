#include "CChunkGenerator.h"
#include <glm/gtc/noise.hpp>
#include "Biome/CBiome.h"
#include <glm/glm.hpp>
#include "../CWorld.h"
#include <iostream>
#include <csignal>

inline float  BilinearInterpolation(float botLeft, float topLeft, float botRight, float topRight, float xMin, float xMax, float yMin, float yMax, float x, float y) 
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


/*
	MUST REFRACTOR IT, I REGRET NOT COMMENTING THIS SPAGHETTA
*/
std::unique_ptr<SBlockInfo[]> CChunkGenerator::generateChunk(int cx, int cy, int cz, CChunkGenerator* Generator)
{
    std::unique_ptr<SBlockInfo[]> output = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
	StructureInfo info; int dirtLevel;

	int downPos = cy * CHUNK_SIZE; /*botton of the chunk*/
	int upPos = cy * CHUNK_SIZE+CHUNK_SIZE; /*top of the chunk*/
	int top = 0;

	static auto getIndex = [](int x, int y, int z)
	{
		return x + CHUNK_SIZE*(y + z * CHUNK_SIZE);
	};

	CBiome *NW, *SW, *NE, *SE;
	int gx = cx*CHUNK_SIZE, gz=cz*CHUNK_SIZE;
	int NW_y = (NW = Generator->getBiome(gx, gz))->simplexPlains(gx, gz);

	gx = cx*CHUNK_SIZE; gz=cz*CHUNK_SIZE+CHUNK_SIZE-1;
	int SW_y = (SW = Generator->getBiome(gx, gz))->simplexPlains(gx, gz);

	gx = cx*CHUNK_SIZE+CHUNK_SIZE-1; gz=cz*CHUNK_SIZE;
	int NE_y = (NE = Generator->getBiome(gx, gz))->simplexPlains(gx, gz);

	gx = cx*CHUNK_SIZE+CHUNK_SIZE-1; gz=cz*CHUNK_SIZE+CHUNK_SIZE-1;
	int SE_y = (SE = Generator->getBiome(gx, gz))->simplexPlains(gx, gz);

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			gx = cx * CHUNK_SIZE + x;//global coord x
			gz = cz * CHUNK_SIZE + z;//global coord z

			/////////////FIND Y POS ON HEIGHTMAP//////////////////
			CBiome* currentBiome = Generator->getBiome(gx, gz);
			int waterLevel = currentBiome->getWaterLevel();
			srand(x+(z<<8));
			if(currentBiome->getDirtLevel() != -1)
				dirtLevel = currentBiome->getDirtLevel() - rand() % 2;
			else dirtLevel = 2000;
			//top = currentBiome->simplexPlains(gx, gz);

			if(NW == SW && NW == NE && NW == SE && NW == currentBiome) top = currentBiome->simplexPlains(gx, gz);
			else top = BilinearInterpolation(SW_y, NW_y, SE_y, NE_y, 0, CHUNK_SIZE-1, CHUNK_SIZE-1, 0, x, z);
			//top += currentBiome->getTopBlock();
			for(int y = 0; y < CHUNK_SIZE; ++y)
			{
				int gy = cy*CHUNK_SIZE + y;
				if(gy <= currentBiome->getWaterLevel())
				{
					output[getIndex(x, y, z)].id = BLOCK_WATER;
				}
			}

			if (downPos < top && top < upPos)
			{
				int max = top - downPos;
				for (int i = 0; i < max; i++)
                    output[getIndex(x, i, z)].id = currentBiome->caveHere(gx, cy*CHUNK_SIZE+i, gz) ? BLOCK_STONE : BLOCK_AIR;
			}
			else if(downPos < top)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
                    output[getIndex(x, i, z)].id = currentBiome->caveHere(gx, cy*CHUNK_SIZE+i, gz) ? BLOCK_STONE : BLOCK_AIR;
			}

			bool underWater = waterLevel >= top;
			if(underWater && top >= downPos && top <  upPos)
			{
				output[getIndex(x, top-downPos, z)].id = BLOCK_SAND;
			}
			bool sandy = top <= waterLevel - 5;

			for(int i = CHUNK_SIZE-1; i >= 0; --i)
			{
				int current = output[getIndex(x, i, z)].id;
				int gy = cy*CHUNK_SIZE+i;
				if(current == BLOCK_STONE)
				{
					int distTop = std::abs(top - gy);
					bool aboveBlockMissing = distTop <= 1 || (!currentBiome->caveHere(gx, gy+1, gz) && distTop <= 30);

					if(aboveBlockMissing)
					{
						if(dirtLevel>= gy)
							output[getIndex(x, i, z)].id = sandy ? BLOCK_SAND : currentBiome->getTopBlock();
						if(i+1<CHUNK_SIZE && output[getIndex(x, i+1, z)].id ==BLOCK_AIR)
						{
							srand(gx^gy^gz);
							if((rand()&15)==0)
								output[getIndex(x, i+1, z)].id = currentBiome->getVegetationBlock(gx, gy, gz);
						}
						if(!underWater)
						currentBiome->AddStructure(gx, gy, gz, m_structInfo, m_mutex);
					}
					else
					{
						for(int j = 0; j <= 5; ++j)
						{
							int _gy = gy+j;
							int _distTop = std::abs(top - _gy);
							bool aboveBlockMissing = _distTop <= 1 || (!currentBiome->caveHere(gx, _gy+1, gz) && _distTop <= 30);
							if(aboveBlockMissing && _gy < dirtLevel)
							{
								output[getIndex(x, i, z)].id = sandy ? BLOCK_SAND : currentBiome->getUnderBlock();
								break;
							}
						}
					}
				}

				//if(underWater && (gy >= top-5 ))
				//	output[getIndex(x, i, z)].id = BLOCK_SAND;
				
				/*if(gy <= waterLevel + 5 && gy >= top-5 && output[getIndex(x, i, z)].id != BLOCK_WATER)
				{
				}*/
			}
		}
	}
    return output;
}

std::shared_ptr<class CChunk> CChunkGenerator::getChunk(int x, int y, int z, CWorld* world)
{
    std::shared_ptr<CChunk> chunk = std::make_shared<CChunk>(glm::ivec3(x,y,z));
	chunk->setManager(&world->getManager());
	if(!chunk->getSaveComponent().load())
	{
		//int biome = m_map.getBiome(x, z,  0.01f);
    	chunk->getVoxelComponent().setAllBlocks(generateChunk(x, y, z, this));
	}
    return chunk;
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
/*
	for(int i = 0; i < TOTAL_BIOMES; ++i)
	{
		m_biomes[i]->consumeCache(world);
	}*/
}

CChunkGenerator::CChunkGenerator() : m_map(0, TOTAL_BIOMES)
{
	m_biomes[BIOME_PLAINS] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_PLAINS))->addStructure(new CStructureTree())->addVegetation(BLOCK_PEBBLE)->setWaterLevel(58));
	m_biomes[BIOME_DESERT] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_DESERT))->setTopBlock(BLOCK_SAND)->setUnderBlock(BLOCK_SAND)->addVegetation(BLOCK_PEBBLE));
	m_biomes[BIOME_OCEAN] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_OCEAN))->setTopBlock(BLOCK_SAND)->setUnderBlock(BLOCK_SAND)->setLevel(16)->setFrequency(0.5));
	m_biomes[BIOME_MOUNTAINS] = std::unique_ptr<CBiome>((new CBiome(this, BIOME_MOUNTAINS))->setLevel(64)->setIntensity(80)->addVegetation(BLOCK_PEBBLE)->setDirtLevel(62)->setFrequency(0.75f));
}

int CChunkGenerator::getTop(int x, int z) const
{
	int cx = std::floor((float)x / (float)CHUNK_SIZE);
	int cz = std::floor((float)z / (float)CHUNK_SIZE);
	return m_biomes[m_map.getBiome(cx, cz, m_biomeScale)]->simplexPlains(x, z);
}


CBiome* CChunkGenerator::getBiome(int cx, int cz) const
{
	return m_biomes[m_map.getBiome(cx, cz, m_biomeScale)].get();
}

CChunkGenerator::~CChunkGenerator()
{
}