#ifndef CBiome_Header
#define CBiome_Header
#include <memory>
#include "../../CChunk.h"
#include "../../CStructure.h"
#include "../../CChunkManager.h"

class CBiome
{
    std::vector<std::unique_ptr<CStructure>> m_structures;
    std::vector<int> m_vegetations;
    class CChunkGenerator* m_parent = nullptr;
    int m_id = 0;

    int m_level = 64;
    int m_intesity = 16;
    int m_topBlock = BLOCK_GRASS;
    int m_underBlock = BLOCK_DIRT;
    int m_waterLevel = 30;
    int m_dirtLevel = -1;
    float m_frequency = 1;
    float m_minForCave = 0.25f;

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
    CBiome* addVegetation(int BlockID);
    CBiome* setMinCave(float minCave);
    CBiome* setDirtLevel(int level);
    CBiome* setFrequency(float level);

    int getVegetationBlock(int x, int y, int z) const;
    int getTopBlock() const;
    int getUnderBlock() const;
    int getWaterLevel() const;
    int getLevel() const;
    int getIntensity() const;
    int getDirtLevel() const;

    float simplexPlains(float x, float z) const;
    float simplexCave(float x, float y, float z) const;
    bool caveHere(int x, int y, int z) const;

    void AddStructure(int gx, int gy, int gz, StructureInfo& info, std::mutex& mutex);

    //virtual std::unique_ptr<SBlockInfo[]> generateChunk(int cx, int cy, int cz, class CChunkGenerator* Generator);
    //void consumeCache(CChunkManager* world);
};

#endif