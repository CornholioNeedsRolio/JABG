#include "CMegaChunkManager.h"
#include "Game/GameWorld/CWorld.h"

std::shared_ptr<CMegaChunk> CMegaChunkManager::GetMegaChunk(int x, int y, int z)
{
			std::lock_guard g(m_mutex);
			for(auto& megachunk : m_megaChunks)
			{
						if(megachunk->PositionEqual(x, y, z)) {
									megachunk->ResetTime();
									return megachunk;
						}
			}
			std::shared_ptr<CMegaChunk> chunk = std::make_shared<CMegaChunk>(x, y, z, m_world);
			chunk->Load();
			m_megaChunks.push_back(chunk);
			return chunk;
}

void CMegaChunkManager::Tick(float DeltaTime)
{
			std::lock_guard g(m_mutex);
			for(auto it = m_megaChunks.begin(); it != m_megaChunks.end(); )
			{
						if((*it)->AddTime(DeltaTime)) {
									(*it)->Save();
									it = m_megaChunks.erase(it);
						}
						else ++it;
			}
}

CMegaChunkManager::CMegaChunkManager(CWorld *world) : m_world(world){}

void CMegaChunkManager::ForceSave()
{
			for(auto it = m_world->getManager().getBeginIterator(); it != m_world->getManager().getEndIterator(); ++it)
			{
						for(auto jt = (*it).second.getBeginIterator(); jt != (*it).second.getEndIterator(); ++jt)
						{
									jt->second->getSaveComponent().save();
						}
			}
			for(auto& chunk : m_megaChunks) {
						std::lock_guard g(m_mutex);
						chunk->Save();
			}
}