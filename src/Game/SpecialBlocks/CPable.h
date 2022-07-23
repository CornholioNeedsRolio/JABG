#ifndef CPable_Header
#define CPable_Header
#include "../CBlock.h"

class CPable : public CBlock
{
    const float m_scale = 0.25;
public:
    CPable();
    std::vector<SVertex> getBlockMeshVertices(CBlock* neighbors[27], std::shared_ptr<class CTextureAtlas> texture, glm::ivec3 position) override;
};

#endif