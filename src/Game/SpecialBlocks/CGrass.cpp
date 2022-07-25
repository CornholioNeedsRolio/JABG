#include "CGrass.h"
#include "Engine/CTextureAtlas.h"


CGrass::CGrass()
{
	m_neededNeighbors.reset();
    constexpr int visibleSides[] = {22, 4, 14, 12, 16, 10, 
    9,       //x == 0 y == 0 z== 1
    11,       //x == 2 y == 0 z== 1
    1,       //x == 1 y == 0 z== 0
    19,       //x == 1 y == 0 z== 2
    };
	for(int i = 0; i < 10; ++i)
		m_neededNeighbors[visibleSides[i]] = 1;
}

uint8_t CGrass::getFaceAt(CBlock* neighbors[27], uint8_t face) const
{
    bool useTopTex = false;
    if(face < 4)
    {
        constexpr int SideToFace[] =  { 19, 1, 11, 9  };
        if(neighbors[SideToFace[face]] == this || neighbors[10] == (CBlock*)BlockDirt)
        useTopTex = true;
    }
    uint8_t currentFace = useTopTex ? BLOCK_TOP : face;
    return getFaceAtlasIndex(currentFace);
}