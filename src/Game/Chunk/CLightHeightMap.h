#pragma once
#include "Game/Chunk/ChunkComponents/CChunkVoxelComponent.h"

class CLightHeightMap
{
    std::array<int, CHUNK_SIZE*CHUNK_SIZE> m_heightMap;
    std::mutex m_mutex;
public:
    CLightHeightMap();
    int getValue(std::uint8_t x, std::uint8_t z);
    void setValue(std::uint8_t x, std::uint8_t z, int value);
    
};
