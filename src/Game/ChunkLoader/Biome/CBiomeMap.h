#ifndef CBiomeMap_Header
#define CBiomeMap_Header
#include <glm/glm.hpp>

class CBiomeMap
{
    const int m_biomeSeed = 0, m_biomeNum = 0;
    glm::vec2 random(glm::vec2 position) const;
    int randomBiome(glm::vec3 position) const;
public:
    CBiomeMap(int biome_seed, int biomeNum);
    int getBiome(int x, int z, float scale) const;
};

#endif