#include "CChunkMeshThreadManager.h"
#include "Game/Chunk/ChunkManager/CChunkManager.h"
#include "Game/GameWorld/CWorld.h"
#include "Game/Chunk/CChunk.h"
#include <iostream>

CChunkMeshThreadManager::CChunkMeshThreadManager(class CWorld* world) : m_world(world)
{
	m_running = true;
	m_flags = 0;
	m_thread = std::thread(&CChunkMeshThreadManager::threadMain, this);
	m_selectedChunk = nullptr;
}

CChunkMeshThreadManager::~CChunkMeshThreadManager()
{
	m_running = false;
	if (m_thread.joinable())
		m_thread.join();
}

void CChunkMeshThreadManager::threadMain()
{
	while (m_running)
	{
		if (!(m_flags & SELECTING_SHIT))
		{
			m_flags |= BUILDING_MESH;
			if (m_selectedChunk)
			{
				//m_selectedChunk->getLightComponent().BuildChunkLightingData(m_neighbors);
				m_selectedChunk->getMeshComponent().BuildMeshData(m_neighbors, m_world->getAtlas());
			}
			m_selectedChunk = nullptr;
			m_neighbors.fill(nullptr);
			m_flags &= ~BUILDING_MESH;
		}
	}
}

void CChunkMeshThreadManager::ChooseChunk()
{
	if (m_flags & BUILDING_MESH)
		return;
	m_flags |= SELECTING_SHIT;
	std::tuple<int, int, int> playerPos = m_playerPos;
	int y;
	int rd = m_renderDistance;
	int px = std::get<0>(playerPos);
	int py = std::get<1>(playerPos);
	int pz = std::get<2>(playerPos);

	for (int i = 0; i <= rd; i++)
	{
		for (int x = px - i; x <= px + i; x++)
		{
			for (int z = pz - i; z <= pz + i; z++)
			{

				CChunkPart* part = m_world->getManager().getChunkPart(x, z);

				if (!part)
					continue;
				CChunk* chunk = part->getChunkInRange(py, false, &y);
				
				if (!chunk) continue;
				if (!chunk->getMeshComponent().IsDirty()) continue;

				m_neighbors.fill(nullptr);
				std::map<std::pair<int, int>, CChunkPart*> Cache;
				for(int xx = -1; xx <= 1; ++xx )
				{
					for(int yy = -1; yy <= 1; ++yy )
					{
						for(int zz = -1; zz <= 1; ++zz )
						{
							if(!xx && !yy && !zz) continue;
							CChunkPart* part =  nullptr;
							std::pair<int, int>	partpos(x+xx, z+zz);
							if(Cache.find(partpos) != Cache.end())
								part = Cache[partpos];
							else{
								part =  m_world->getManager().getChunkPart(x+xx, z+zz);
								Cache.insert({partpos, part});
							}
							if(!part) continue;
							CChunk* chunk = part->getChunk(y+yy);
							if(chunk) m_neighbors[(xx+1) + 3 * (yy+1 + (zz+1)*3)] = chunk; 
						}
					}
				}

				m_selectedChunk = chunk;
				m_flags &= ~SELECTING_SHIT;
				return;
			}
		}
	}
	m_flags &= ~SELECTING_SHIT;
}

void CChunkMeshThreadManager::setPlayerPos(std::tuple<int, int, int> playerPos, int renderDistance)
{
	m_playerPos = playerPos;
	m_renderDistance = renderDistance;
}
