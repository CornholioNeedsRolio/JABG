#ifndef CChunkGenerator_Header
#define CChunkGenerator_Header
#include <memory>
#include <vector>
#include "../CChunk.h"
#include "Biome/CBiome.h"
#include "../CStructure.h"
#include "Biome/CBiomeMap.h"

class CChunkGenerator
{
    enum
    {
        BIOME_PLAINS,
        BIOME_DESERT,
        BIOME_OCEAN,
        TOTAL_BIOMES
    };
    CBiomeMap m_map;
    std::array<std::unique_ptr<CBiome>, 3> m_biomes; 
public:
    CChunkGenerator();
    ~CChunkGenerator();
    
    CBiome* getBiome(int cx, int cz) const;
    int getTop(int x, int z) const;
    std::shared_ptr<CChunk> getChunk(int x, int y, int z, class CWorld* world);

    void consumeCache(CChunkManager* world);
};

#endif