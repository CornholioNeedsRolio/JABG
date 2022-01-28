#ifndef CChunkGenerator_Header
#define CChunkGenerator_Header
#include <memory>
#include "../CChunk.h"
#include "../CStructure.h"

class CChunkGenerator
{
    float OctaveSimplex(float x, float y, float z, int octaves, float persistence);
    int m_max = 1;
    virtual std::unique_ptr<SBlockInfo[]> generateChunk(int cx, int cy, int cz);
    std::vector<std::unique_ptr<CStructure>> m_structures; //why no smart pointer? so I can mutex their destruction, so no crash happens
    std::mutex m_mutex;
    std::mutex m_destruction;
    StructureInfo m_structInfo;
public:
    CChunkGenerator();
    ~CChunkGenerator();

    std::shared_ptr<CChunk> getChunk(int x, int y, int z, class CWorld* world);
    int getMax() const;

    void consumeCache(CChunkManager* world);
};

#endif