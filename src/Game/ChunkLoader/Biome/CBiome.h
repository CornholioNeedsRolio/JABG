#ifndef CBiome_Header
#define CBiome_Header
#include <memory>
#include "../../CChunk.h"
#include "../../CStructure.h"
#include "../../CChunkManager.h"

class CBiome
{
    std::vector<std::unique_ptr<CStructure>> m_structures;
    std::mutex m_mutex;
    std::mutex m_destruction;
    StructureInfo m_structInfo;
    class CChunkGenerator* m_parent = nullptr;
    int m_id = 0;

    int m_level = 64;
    int m_intesity = 16;
    int m_topBlock = BLOCK_GRASS;
    int m_underBlock = BLOCK_DIRT;
    int m_waterLevel = 69;

    float OctaveSimplex(float x, float y, float z, int octaves, float persistence) const;
public:
    CBiome(class CChunkGenerator* parent, int id);
    ~CBiome();
    CBiome* addStructure(CStructure* structure);
    CBiome* setTopBlock(int topBlock);
    CBiome* setUnderBlock(int underBlock);
    CBiome* setWaterLevel(int waterLevel);
    CBiome* setLevel(int level);
    CBiome* setIntensity(int intensity);

    float simplexPlains(float x, float z) const;
    float simplexCave(float x, float y, float z) const;

    virtual std::unique_ptr<SBlockInfo[]> generateChunk(int cx, int cy, int cz);
    void consumeCache(CChunkManager* world);
};

#endif