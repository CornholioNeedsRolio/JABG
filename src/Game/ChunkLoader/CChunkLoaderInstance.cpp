#include "CChunkLoaderInstance.h"
#include "CChunkGenerator.h"
#include "CChunkLoader.h"
#include <iostream>

CChunkLoaderInstance::CChunkLoaderInstance(CChunkLoader* parent) : m_parent(parent)
{
    m_thread = std::thread(&CChunkLoaderInstance::Run, this);
    m_flags = 0;
}

CChunkLoaderInstance::~CChunkLoaderInstance()
{

}

void CChunkLoaderInstance::addTargetChunk(int x, int y, int z)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    for(int i = 0; i < m_targetChunks.size(); ++i)
        if(std::get<0>(m_targetChunks[0]) == x && std::get<1>(m_targetChunks[0]) == y && std::get<2>(m_targetChunks[0]) == z)
            return;
    m_targetChunks.emplace_back(x, y, z);
    m_chunkNum = m_targetChunks.size();
}

void CChunkLoaderInstance::joinThreads()
{
    markDeath();
    if(m_thread.joinable())
        m_thread.join();
}

int CChunkLoaderInstance::getChunkNum()
{
    return m_chunkNum;
}

void CChunkLoaderInstance::markDeath()
{
    m_flags |= SHOULD_DIE;
}

void CChunkLoaderInstance::Run()
{
    while(!(m_flags & SHOULD_DIE))
    {
        {
            const std::lock_guard<std::mutex> lock(m_mutex);
            if(m_targetChunks.empty()) continue;

			m_current = std::move(m_targetChunks[0]);
            m_targetChunks.erase(m_targetChunks.begin());
            m_chunkNum = m_targetChunks.size();
        }
        m_parent->addToList(m_parent->getGenerator().getChunk(std::get<0>(m_current), std::get<1>(m_current), std::get<2>(m_current), m_parent->getWorld()));
    }
}

std::tuple<int, int, int>  CChunkLoaderInstance::clearTargetChunks()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_targetChunks.clear();
	return m_current;
}

std::vector<std::tuple<int, int, int>> CChunkLoaderInstance::getTargetChunks()
{
			const std::lock_guard<std::mutex> lock(m_mutex);
			return m_targetChunks;
}

