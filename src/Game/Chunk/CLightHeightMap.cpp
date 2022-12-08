#include "CLightHeightMap.h"

CLightHeightMap::CLightHeightMap()
{
    m_heightMap.fill(0);
}

int CLightHeightMap::getValue(std::uint8_t x, std::uint8_t z)
{
    std::lock_guard g(m_mutex);
    return m_heightMap[x + CHUNK_SIZE*z];
}

void CLightHeightMap::setValue(std::uint8_t x, std::uint8_t z, int value)
{
    std::lock_guard g(m_mutex);
    m_heightMap[x + CHUNK_SIZE*z] = value;
}
