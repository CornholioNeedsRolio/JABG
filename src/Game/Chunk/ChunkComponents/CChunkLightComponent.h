﻿#ifndef CChunkLightComponent_Header
#define CChunkLightComponent_Header
#include <array>
#include "CChunkVoxelComponent.h"

#include "IChunkComponent.h"

struct SCubeightData
{
    std::int16_t SunValue = 0;
};

struct SBlockLightData
{
    std::array<std::int16_t, 6> FaceLight;
};

class CChunkLightComponent : public IChunkComponent
{
    class CChunk* m_parent;
    std::array<SCubeightData, CHUNK_SIZE_SQR> m_blockData;
    std::array<SBlockLightData, CHUNK_SIZE_SQR> m_blockLightData;
    std::mutex m_mutex;

    SCubeightData getData(std::int16_t x, std::int16_t y, std::int16_t z, const std::array<CChunk*, 27>* neighbors);
    SCubeightData& getDataRef(std::int16_t x, std::int16_t y, std::int16_t z);

    int getIndex(std::int16_t x, std::int16_t y, std::int16_t z) const;
    
    struct smh {int chunk_x, chunk_y, chunk_z; int local_x, local_y, local_z;};
    smh getStuff(std::int16_t x, std::int16_t y, std::int16_t z) const;

    std::int16_t MaxValueOfNeighbors(std::int16_t x, std::int16_t y, std::int16_t z, const std::array<CChunk*, 27>& neighbors);
        
    void PrepareLightLevels(const std::array<CChunk*, 27>& neighbors);
    void PropagateLightLevels(const std::array<CChunk*, 27>& neighbors);
    void PrepareDataForBlocks(const std::array<CChunk*, 27>& neighbors);
public:
    CChunkLightComponent(class CChunk* Parent);
    
    void BuildChunkLightingData(const std::array<CChunk*, 27>& neighbors);
    SBlockLightData GetBlockData(std::int16_t x, std::int16_t y, std::int16_t z) const;
};

#endif