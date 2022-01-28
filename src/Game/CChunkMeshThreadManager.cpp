#include "CChunkMeshThreadManager.h"
#include "CChunkManager.h"
#include "CWorld.h"
#include "CChunk.h"
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
				int pos;
				CChunk* chunk = part->getChunkInRange(py, false, &pos);

				if (!chunk)
					continue;
					
				if (!chunk->getMeshComponent().isDirty())
					continue;

				m_neighbors.fill(nullptr);


				CChunkPart* S = m_world->getManager().getChunkPart(x, z-1);
				CChunkPart* N = m_world->getManager().getChunkPart(x, z+1);
				CChunkPart* W = m_world->getManager().getChunkPart(x-1, z);
				CChunkPart* E = m_world->getManager().getChunkPart(x+1, z);
				
				//if(!S || !N || !W || !E) continue;

				m_neighbors[CHUNKFACE_TOP] = part->getChunk(pos+1);
				m_neighbors[CHUNKFACE_BOT] = part->getChunk(pos-1);
				
				if(W) m_neighbors[CHUNKFACE_LEFT] = W->getChunk(pos);
				//if (!m_neighbors[CHUNKFACE_LEFT]) continue;

				if(E) m_neighbors[CHUNKFACE_RIGHT] = E->getChunk(pos);
				//if (!m_neighbors[CHUNKFACE_RIGHT]) continue;

				if(N) m_neighbors[CHUNKFACE_FRONT] = N->getChunk(pos);
				//if (!m_neighbors[CHUNKFACE_FRONT]) continue;

				if(S) m_neighbors[CHUNKFACE_BACK] = S->getChunk(pos);
				//if (!m_neighbors[CHUNKFACE_BACK]) continue;

				m_selectedChunk = chunk;
				//printf("%i %i %i\n", x, pos, z);
				m_flags &= ~SELECTING_SHIT;
				return;
			}
		}
	}
	//printf("looking");
	m_flags &= ~SELECTING_SHIT;
}

void CChunkMeshThreadManager::setPlayerPos(std::tuple<int, int, int> playerPos, int renderDistance)
{
	m_playerPos = playerPos;
	m_renderDistance = renderDistance;
}
