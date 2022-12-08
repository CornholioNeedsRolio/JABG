#include "CChunkLightComponent.h"

#include <iostream>

#include "Game/Chunk/CChunk.h"
#include "Game/Chunk/CChunkPart.h"

SCubeightData& CChunkLightComponent::getDataRef(std::int16_t x, std::int16_t y, std::int16_t z)
{
    std::lock_guard g(m_mutex);
    return m_blockData[getIndex(x, y, z)];
}

int CChunkLightComponent::getIndex(std::int16_t x, std::int16_t y, std::int16_t z) const
{
    return x + CHUNK_SIZE*(y+CHUNK_SIZE*z);
}

std::int16_t CChunkLightComponent::MaxValueOfNeighbors(std::int16_t x, std::int16_t y, std::int16_t z, const std::array<CChunk*, 27>& neighbors)
{
    std::int16_t maxValue = 0;
    for(std::int16_t xx = -1; xx <= 1; ++xx)
    {
        for(std::int16_t zz = -1; zz <= 1; ++zz)
        {
            for(std::int16_t yy = -1; yy <= 1; ++yy)
            {
                if(!xx && !yy && !zz) continue;
                maxValue = std::max(getData(x + xx, y + yy, z + zz, &neighbors).SunValue, maxValue);
            }
        }
    }
    return maxValue;
}

void CChunkLightComponent::PrepareLightLevels(const std::array<CChunk*, 27>& neighbors)
{
    CChunkPart* part = m_parent->getChunkPart();
    CLightHeightMap* HeightMap = part->getNormalMap(); 

    for(std::int16_t x = 0; x < CHUNK_SIZE; ++x)
    {
        for(std::int16_t z = 0; z < CHUNK_SIZE; ++z)
        {
            //int16_t maxY = HeightMap->getValue(x, z)-CHUNK_SIZE*m_parent->getChunkPosition().y;
            //if(maxY <= 0) maxY = CHUNK_SIZE;
            //std::cerr << maxY << " ";
            for(std::int16_t y = 0; y < CHUNK_SIZE; ++y)
            {
                int gy = m_parent->getChunkPosition().y * CHUNK_SIZE + y;
                getDataRef(x, y, z).SunValue = gy >= HeightMap->getValue(x, z) ? 15 : 0;
            }
        }
    }
}

void CChunkLightComponent::PropagateLightLevels(const std::array<CChunk*, 27>& neighbors)
{
    bool changesDone = true;
    while(changesDone)
    {
        changesDone = false;
        for(std::int16_t x = 0; x < CHUNK_SIZE; ++x)
        {
            for(std::int16_t z = 0; z < CHUNK_SIZE; ++z)
            {
                for(std::int16_t y = 0; y < CHUNK_SIZE; ++y)
                {
                    int maxValue = MaxValueOfNeighbors(x, y, z, neighbors);
                    if(maxValue <= 0) continue;
                    if(!m_parent->getVoxelComponent().getBlock(x, y, z).getBlock()->isTransparent()) continue;
                    SCubeightData& ref = getDataRef(x, y, z);
                    if(maxValue - 1 > ref.SunValue)
                    {
                        ref.SunValue = maxValue - 1;
                        changesDone = true;
                    }
                }
            }
        }
    }
}

void CChunkLightComponent::PrepareDataForBlocks(const std::array<CChunk*, 27>& neighbors)
{
    for(std::int16_t x = 0; x < CHUNK_SIZE; ++x)
    {
        for(std::int16_t z = 0; z < CHUNK_SIZE; ++z)
        {
            for(std::int16_t y = 0; y < CHUNK_SIZE; ++y)
            {
                m_blockLightData[getIndex(x, y, z)].FaceLight[CBlock::BLOCK_TOP]    = getData(x     , y + 1 , z     , &neighbors).SunValue;
                m_blockLightData[getIndex(x, y, z)].FaceLight[CBlock::BLOCK_BOTTOM] = getData(x     , y - 1 , z     , &neighbors).SunValue;
        
                m_blockLightData[getIndex(x, y, z)].FaceLight[CBlock::BLOCK_LEFT]   = getData(x - 1 , y     , z     , &neighbors).SunValue;
                m_blockLightData[getIndex(x, y, z)].FaceLight[CBlock::BLOCK_RIGHT]  = getData(x + 1 , y     , z     , &neighbors).SunValue;
                        
                m_blockLightData[getIndex(x, y, z)].FaceLight[CBlock::BLOCK_FRONT] = getData(x      , y     , z - 1 , &neighbors).SunValue;
                m_blockLightData[getIndex(x, y, z)].FaceLight[CBlock::BLOCK_BACK]  = getData(x      , y     , z - 1 , &neighbors).SunValue;
            }
        }
    }
}

CChunkLightComponent::CChunkLightComponent(CChunk* Parent) : m_parent(Parent)
{
}

void CChunkLightComponent::BuildChunkLightingData(const std::array<CChunk*, 27>& neighbors)
{
    PrepareLightLevels(neighbors);
    PropagateLightLevels(neighbors);
    PrepareDataForBlocks(neighbors);
}

SBlockLightData CChunkLightComponent::GetBlockData(std::int16_t x, std::int16_t y, std::int16_t z) const
{
    return m_blockLightData[getIndex(x, y, z)];
}

CChunkLightComponent::smh CChunkLightComponent::getStuff(std::int16_t x, std::int16_t y, std::int16_t z) const
{
    int chunk_x = 1, chunk_y = 1, chunk_z = 1;
    int local_x = x, local_y = y, local_z = z;
    if(z <= -1) {
        --chunk_z; 	local_z = CHUNK_SIZE-1;
    }else if(z >= CHUNK_SIZE) {
        ++chunk_z; local_z = 0;
    }

    if(x <= -1) {
        --chunk_x;  local_x = CHUNK_SIZE-1;
    } else if(x >= CHUNK_SIZE) {
        ++chunk_x; local_x = 0;
    }

    if(y <= -1) {
        --chunk_y;  local_y = CHUNK_SIZE-1;
    } else if(y >= CHUNK_SIZE) {
        ++chunk_y; local_y = 0;
    }
    return { chunk_x, chunk_y, chunk_z, local_x, local_y, local_z};
}

SCubeightData CChunkLightComponent::getData(std::int16_t x, std::int16_t y, std::int16_t z, const std::array<CChunk*, 27>* neighbors)
{
    const auto temp = getStuff(x, y, z);
    const int index = temp.chunk_x + 3*(temp.chunk_y+temp.chunk_z*3);
    if(index == 13)
    {
        std::lock_guard g(m_mutex);
        return  m_blockData[x + CHUNK_SIZE*(y+CHUNK_SIZE*z)];
    }
    if(neighbors)
    {
        if((*neighbors)[index])
        {
            std::lock_guard g(m_mutex);
            return (*neighbors)[index]->getLightComponent().getData(temp.local_x, temp.local_y, temp.local_z, nullptr);
        }
    }
		
    return {0};
}