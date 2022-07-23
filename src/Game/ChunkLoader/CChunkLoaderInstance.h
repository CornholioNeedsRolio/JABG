#ifndef CChunkLoaderInstance_Header
#define CChunkLoaderInstance_Header
#include <tuple>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include "../CChunk.h"

class CChunkLoaderInstance
{
    std::atomic_uint8_t m_flags = 0;
    std::atomic_uint16_t m_chunkNum = 0;
    std::vector<std::tuple<int,int,int>> m_targetChunks;
    std::mutex m_mutex;
    std::thread m_thread;
    class CChunkLoader* m_parent = nullptr;
    enum
    {
        SHOULD_DIE = 0x1,
    };
public:
    CChunkLoaderInstance(CChunkLoader* parent);
    ~CChunkLoaderInstance();
    void addTargetChunk(int x, int y, int z);
    void clearTargetChunks();
    void markDeath();
    int getChunkNum();
    void joinThreads();
    void Run();
};

#endif