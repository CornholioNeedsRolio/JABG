#ifndef CChunkLoader_Header
#define CChunkLoader_Header
#include <vector>
#include <thread>
#include <mutex>
#include "CChunkGenerator.h"
#include "CChunkLoaderInstance.h"

class CChunkLoader
{
    std::vector<std::unique_ptr<CChunkLoaderInstance>> m_instances;
    std::vector<std::shared_ptr<CChunk>> m_chunks;
    std::mutex m_mutex;
    class CWorld* m_world = nullptr;
    CChunkGenerator m_generator;
public:
    CChunkLoader(int threadNum, CWorld* world);
    ~CChunkLoader();

    CWorld* getWorld();
    void requestChunk(int x, int y, int z);
    void addToList(std::shared_ptr<CChunk> chunk);
    void emptyListAllThreads();
    void onPlayerMove(std::tuple<int, int, int> playerPos, int distance, CChunkManager* world);
    void Tick(CChunkManager* world);
    void joinThreads();
    CChunkGenerator& getGenerator();
};

#endif