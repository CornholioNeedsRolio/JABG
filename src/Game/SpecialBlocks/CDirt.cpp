#include "CDirt.h"
#include "../../CTextureAtlas.h"

CDirt::CDirt() : CBlock()
{

}

constexpr int visibleSides[] = {22, 4, 14, 12, 16, 10};

uint8_t CDirt::getFaceAt(CBlock* neighbors[27], uint8_t face) const
{
    return  (face < 4 && neighbors[16] == (CBlock*)BlockGrass) ? m_abovegrassfaces[face] : getFaceAtlasIndex(face); ;
}