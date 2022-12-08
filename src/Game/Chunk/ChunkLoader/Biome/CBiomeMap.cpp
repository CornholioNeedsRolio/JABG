#include "CBiomeMap.h"
#include "../CChunkLoader.h"

CBiomeMap::CBiomeMap(int biome_seed, int biomeNum) : m_biomeSeed{biome_seed}, m_biomeNum{biomeNum}
{

}

int CBiomeMap::getBiome(int x, int z, float scale) const
{
    glm::vec2 pos = glm::vec2(x, z) * scale;
    glm::vec2 f_pos = glm::fract(pos);
    glm::vec2 g_pos = glm::floor(pos);

    float min = 1.f; 
    int biome = 0;
    for(int lx = -1; lx <= 1; ++lx)
    {
        for(int lz = -1; lz <= 1; ++lz)
        {
            glm::vec2 neighbor(lx, lz);
            glm::vec2 neighbor_value = random(g_pos+neighbor);

            glm::vec2 diff =  neighbor+neighbor_value-f_pos;
            float length = glm::length(diff);
            if(min > length)
            {
                min = length;

                glm::vec2 neighbor_pos = glm::vec2(g_pos + neighbor);
                biome = randomBiome(glm::vec3(neighbor_pos.x, neighbor_pos.y, m_biomeSeed));
            }
        }
    }

    return biome;
}


glm::vec2 CBiomeMap::random(glm::vec2 position) const
{
    return glm::fract(glm::sin(glm::vec2(glm::dot(position,glm::vec2(0.470,-0.370)), glm::dot(position,glm::vec2(0.040,-0.240))))*6942.5f);
}

int CBiomeMap::randomBiome(glm::vec3 position) const
{
    int max = m_biomeNum + 1;
    int output = ((int)(glm::abs(position.x*max*max + position.y*max + position.z)))%m_biomeNum;
    return output;
}