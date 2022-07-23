#include "CChunkLoader.h"
#include "../CChunkPart.h"
#include "../CChunkManager.h"

CChunkLoader::CChunkLoader(int threadNum, CWorld* world) : m_world(world)
{
    m_instances.reserve(threadNum);
    for(int i = 0; i < threadNum; ++i)
        m_instances.push_back(std::make_unique<CChunkLoaderInstance>(this));
}

CChunkLoader::~CChunkLoader()
{
}

void CChunkLoader::requestChunk(int x, int y, int z)
{
    //search the thread with lowest amount of chunks requested
    int min_index = 0;
    for(int i = 1; i < m_instances.size(); ++i)
        if(m_instances[min_index]->getChunkNum() > m_instances[i]->getChunkNum())
            min_index = i;
    //give it the job
    m_instances[min_index]->addTargetChunk(x, y, z);
}

void CChunkLoader::emptyListAllThreads()
{
    for(int i = 0; i < m_instances.size(); ++i)
        m_instances[i]->clearTargetChunks();
}

void CChunkLoader::addToList(std::shared_ptr<CChunk> chunk)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_chunks.push_back(chunk);
}

CWorld* CChunkLoader::getWorld()
{
    return m_world;
}

void CChunkLoader::onPlayerMove(std::tuple<int, int, int> playerPos, int distance, CChunkManager* world)
{
    emptyListAllThreads();
    int cx = std::get<0>(playerPos);
    int cy = std::get<1>(playerPos);
    int cz = std::get<2>(playerPos);
    distance = distance*0.5+1;

    CChunkPart* part = nullptr;
    CChunk* chunk = nullptr;

    for (int i = 0; i <= distance; i++)
    {
        for(int x = cx-i; x <= cx+i; ++x)
        {
            for(int z = cz-i; z <= cz+i; ++z)
            {
                part = world->getChunkPart(x, z);
                for(int y = cy-i; y <= cy+i; ++y)
                {
                    if(part)
                        chunk = part->getChunk(y);
                    if(!part || !chunk)
                        requestChunk(x, y, z);
                }
            }
        }
    }
}

void CChunkLoader::joinThreads()
{
    for(auto& instance : m_instances)
        instance->joinThreads();
}

void CChunkLoader::Tick(CChunkManager* world)
{
    {
        const std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_chunks.empty())
        {
            //add chunks to world
            for(auto& chunk : m_chunks)
            {
                glm::ivec3 position = chunk->getChunkPosition();
                CChunkPart* part = world->createChunkIfNone(position.x, position.z);
                part->addChunk(chunk);
            }
            //empty array
            m_chunks.clear();
        }
    }
    m_generator.consumeCache(world);
}

CChunkGenerator& CChunkLoader::getGenerator()
{
    return m_generator;
}