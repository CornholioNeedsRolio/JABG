#ifndef CChunkVoxelComponent_Header
#define CChunkVoxelComponent_Header
#include <memory>
#include <mutex>
#include "Game/CBlock.h"
#include "IChunkComponent.h"

using blocktype = unsigned short;
#define CHUNK_SIZE 16
#define CHUNK_SIZE_SQR CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE
#define HALF_CHUNK CHUNK_SIZE*0.5

struct SBlockInfo
{
    blocktype id;
    CBlock* getBlock() const 
    {
        return BLOCK_DATABASE::getBlock(id);
    };
};

class CChunkVoxelComponent : public IChunkComponent
{
    class CChunk* m_parent = nullptr;
    std::unique_ptr<SBlockInfo[]> m_blocks = nullptr;
    SBlockInfo m_majority{0};
    std::mutex m_mutex;
public:
    CChunkVoxelComponent(CChunk* parent);

    const SBlockInfo& getBlock_nothreadsafe(std::uint8_t x, std::uint8_t y, std::uint8_t z);
    const SBlockInfo& getBlock(std::uint8_t x, std::uint8_t y, std::uint8_t z);
    void setBlock(std::uint8_t x, std::uint8_t y, std::uint8_t z, blocktype id);
    void setAllBlocks(std::unique_ptr<SBlockInfo[]> blocks);
    void setBlocksInBulk(const std::vector<std::pair<std::tuple<int, int, int>, int>>& blocks);
    SBlockInfo* getAllBlocks();
};

#endif