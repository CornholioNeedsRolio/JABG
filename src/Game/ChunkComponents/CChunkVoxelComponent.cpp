#include "CChunkVoxelComponent.h"
#include "../CChunk.h"

CChunkVoxelComponent::CChunkVoxelComponent(CChunk* parent) : m_parent(parent)
{

}

const SBlockInfo& CChunkVoxelComponent::getBlock(char x, char y, char z)
{
    std::lock_guard g(m_mutex);
    if(m_blocks)
        return m_blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE*z)];
    return m_majority;
}

const SBlockInfo& CChunkVoxelComponent::getBlock_nothreadsafe(char x, char y, char z)
{
    if(m_blocks)
        return m_blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE*z)];
    return m_majority;
}

void CChunkVoxelComponent::setBlock(char x, char y, char z, blocktype id)
{
    if (x == 0)
		m_parent->makeNeighborDirty(CARD_S);
	if (x == CHUNK_SIZE - 1)
		m_parent->makeNeighborDirty(CARD_N);
	if (z == 0)
		m_parent->makeNeighborDirty(CARD_W);
	if (z == CHUNK_SIZE - 1)
		m_parent->makeNeighborDirty(CARD_E);
	
	if (y == 0)
		m_parent->makeNeighborDirty(CARD_DOWN);
	if (y == CHUNK_SIZE - 1)
		m_parent->makeNeighborDirty(CARD_UP);
    {
        std::lock_guard g(m_mutex);
        if(!m_blocks)
            m_blocks = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
        m_blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE*z)].id = id;
    }
	m_parent->MakeDirty();
}

void CChunkVoxelComponent::setBlocksInBulk(const std::vector<std::pair<std::tuple<int, int, int>, int>>& blocks)
{
    std::lock_guard g(m_mutex);
    char cards = 0;
    for(auto& block : blocks)
    {
        char x = std::get<0>(block.first);
        char y = std::get<1>(block.first);
        char z = std::get<2>(block.first);

        if(!x) cards |= 1;
        if(x == CHUNK_SIZE - 1) cards |= 2;
        if(!y) cards |= 4;
        if(y == CHUNK_SIZE - 1) cards |= 8;
        if(!z) cards |= 16;
        if(z == CHUNK_SIZE - 1) cards |= 32;

        m_blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE*z)].id = block.second;
    }
    if (cards & 1) m_parent->makeNeighborDirty(CARD_S);
	if (cards & 2) m_parent->makeNeighborDirty(CARD_N);
	if (cards & 4) m_parent->makeNeighborDirty(CARD_W);
	if (cards & 8) m_parent->makeNeighborDirty(CARD_E);
	if (cards & 16) m_parent->makeNeighborDirty(CARD_DOWN);
	if (cards & 32) m_parent->makeNeighborDirty(CARD_UP);

	m_parent->MakeDirty();
}

void CChunkVoxelComponent::setAllBlocks(std::unique_ptr<SBlockInfo[]> blocks)
{
    std::lock_guard g(m_mutex);
    m_blocks = std::move(blocks);
}

SBlockInfo* CChunkVoxelComponent::getAllBlocks()
{
    return m_blocks.get();
}