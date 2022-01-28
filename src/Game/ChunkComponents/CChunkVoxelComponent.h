#ifndef CChunkVoxelComponent_Header
#define CChunkVoxelComponent_Header
#include <memory>
#include <mutex>
#include "../CBlock.h"

using blocktype = unsigned short;
#define CHUNK_SIZE 16

struct SBlockInfo
{
    blocktype id;
    CBlock* getBlock() const 
    {
        return BLOCK_DATABASE::getBlock(id);
    };
};

class CChunkVoxelComponent
{
    class CChunk* m_parent = nullptr;
    std::unique_ptr<SBlockInfo[]> m_blocks = nullptr;
    SBlockInfo m_majority{0};
    std::mutex m_mutex;
public:
    CChunkVoxelComponent(CChunk* parent);

    const SBlockInfo& getBlock_nothreadsafe(char x, char y, char z);
    const SBlockInfo& getBlock(char x, char y, char z);
    void setBlock(char x, char y, char z, blocktype id);
    void setAllBlocks(std::unique_ptr<SBlockInfo[]> blocks);
    void setBlocksInBulk(const std::vector<std::pair<std::tuple<int, int, int>, int>>& blocks);
    SBlockInfo* getAllBlocks();
};

#endif