#include <algorithm>
#include <iostream>
#include "CChunkLoader.h"
#include "../CChunkPart.h"
#include "Game/ChunkManager/CChunkManager.h"

CChunkLoader::CChunkLoader(int threadNum, CWorld* world) : m_world(world)
{
			m_instances.reserve(threadNum);
			for(int i = 0; i < threadNum; ++i)
						m_instances.push_back(std::make_unique<CChunkLoaderInstance>(this));
}

CChunkLoader::~CChunkLoader()
{
}
//MAIN THREAD
void CChunkLoader::requestChunk(int x, int y, int z)
{
			glm::ivec3 currentPos = glm::ivec3(x, y, z);
			for(auto& requestedChunk : m_requestedChunks) {
						if (requestedChunk == currentPos)
									return;
			}
			m_requestedChunks.emplace_back(x, y, z);
			int min_index = 0;
			//search the thread with lowest amount of chunks requested
			for(int i = 1; i < m_instances.size(); ++i) {
						if (m_instances[min_index]->getChunkNum() > m_instances[i]->getChunkNum())
									min_index = i;
			}
			//give it the job
			m_instances[min_index]->addTargetChunk(x, y, z);
}

void CChunkLoader::emptyListAllThreads()
{
			m_requestedChunks.clear();
			for(auto & instance : m_instances) {
						std::tuple<int, int, int> temp = instance->clearTargetChunks();
						m_requestedChunks.emplace_back(std::get<0>(temp), std::get<1>(temp), std::get<2>(temp));
			}
}

void CChunkLoader::addToList(const std::shared_ptr<CChunk>& chunk)
{
			const std::lock_guard<std::mutex> lock(m_mutex);
			m_chunks.push_back(chunk);
}

CWorld* CChunkLoader::getWorld()
{
			return m_world;
}

//MAIN THREAD
void CChunkLoader::onPlayerMove(std::tuple<int, int, int> playerPos, int distance, CChunkManager* world)
{
			emptyListAllThreads();
			int cx = std::get<0>(playerPos);
			int cy = std::get<1>(playerPos);
			int cz = std::get<2>(playerPos);
			distance = (int)(distance*0.5)-2;

			CChunkPart* part;
			CChunk* chunk;

			for (int i = 0; i < distance; i++)
			{
						for(int x = cx-i; x <= cx+i; ++x)
						{
									for(int z = cz-i; z <= cz+i; ++z)
									{
												part = world->getChunkPart(x, z);
												for(int y = cy-i; y <= cy+i; ++y)
												{
															chunk = part ? part->getChunk(y) : nullptr;
															if(!chunk)
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
									for(auto it = m_chunks.begin(); it != m_chunks.end();)
									{
												glm::ivec3 position = (*it)->getChunkPosition();
												CChunkPart* part = world->createChunkIfNone(position.x, position.z);
												/*if(!part->getChunk((*it)->getChunkPosition().y))
															part->addChunk((*it));
												else std::cout << position.x << " " << position.y << " " << position.z << std::endl;*/
												//WHYYYY, WHY ARE THERE DUPLICATE CHUNKS..... no matter how many checks I put in place it fucks me over...
												//future me here... when clearing m_requestedChunks, we should add back the one processed currently... damn threading is so tricky to debug
												//m_requestedChunks.erase( std::remove_if(m_requestedChunks.begin(), m_requestedChunks.end(),[&] (const glm::ivec3& A) -> bool{ return (*it)->getChunkPosition() == A; }), m_requestedChunks.end());
												part->addChunk((*it));
												it = m_chunks.erase(it);
									}
									m_requestedChunks.clear();
						}
			}
			m_generator.consumeCache(world);
}

CChunkGenerator& CChunkLoader::getGenerator()
{
			return m_generator;
}