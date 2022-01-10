#include "CChunkGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>


std::unique_ptr<blocktype[]> CChunkGenerator::generateChunk(int cx, int cy, int cz)
{
    std::unique_ptr<blocktype[]> output = std::make_unique<blocktype[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
	int downPos = cy * CHUNK_SIZE; /*botton of the chunk*/
	int upPos = cy * CHUNK_SIZE+CHUNK_SIZE; /*top of the chunk*/
	int top = 0;
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
                    output[x + i * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = (top-5 < downPos+i) ? BLOCK_DIRT : BLOCK_STONE;
                output[x + max * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = BLOCK_GRASS;
			}
			else if(downPos < top)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
                    output[x + i * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = BLOCK_STONE;
				}
			}
		}
	}

    return output;
}

int CChunkGenerator::getMax() const
{
	return m_max;
}

std::shared_ptr<class CChunk> CChunkGenerator::getChunk(int x, int y, int z)
{
    std::shared_ptr<CChunk> chunk = std::make_shared<CChunk>(glm::ivec3(x,y,z));
    chunk->setAllBlocks(generateChunk(x, y, z));
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