#ifndef CChunkGenerator_Header
#define CChunkGenerator_Header
#include <memory>
#include "../CChunk.h"

class CChunkGenerator
{
    float OctaveSimplex(float x, float y, float z, int octaves, float persistence);
    int m_max = 1;
    virtual std::unique_ptr<blocktype[]> generateChunk(int cx, int cy, int cz);
public:
    std::shared_ptr<CChunk> getChunk(int x, int y, int z);
    int getMax() const;
};

#endif