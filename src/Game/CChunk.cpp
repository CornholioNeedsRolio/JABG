#include "CChunk.h"
#include "CChunkCollider.h"
#include "CChunkPart.h"
#include "CChunkManager.h"
#include <iostream>
#include "CWorld.h"
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include "../CFrustumCollider.h"
#include "../BulkRenderer/CBulkRenderer.h"

CChunk::CChunk(glm::ivec3 position, CChunkManager* manager) :
	m_position(position), m_manager(manager), m_voxelComponent(this), m_meshComponent(this), m_saveComponent(this)
{
}

CChunkVoxelComponent& CChunk::getVoxelComponent()
{
	return m_voxelComponent;
}

CChunkMeshComponent& CChunk::getMeshComponent()
{
	return m_meshComponent;
}

CChunkSaveComponent CChunk::getSaveComponent()
{
	return m_saveComponent;
}

void CChunk::setManager(class CChunkManager* manager)
{
	m_manager = manager;
	makeNeighborsDirty();
}

CChunk::~CChunk()
{
	if(m_manager)
	{
		m_saveComponent.save(m_manager->getWorld()->getFilePath());
	}
}

bool CChunk::fustumCulling(class CCamera* camera) const
{
	if(!camera) return true;
	return camera->getCollider().sphereIntersection((m_position * CHUNK_SIZE + (int)(CHUNK_SIZE * 0.5)), (float)CHUNK_SIZE*1.25);
}

void CChunk::BulkDraw(class CBulkRenderer* renderer) 
{
	m_meshComponent.Draw(renderer);
}

const glm::ivec3& CChunk::getChunkPosition() const
{
	return m_position;
}

void CChunk::makeNeighborDirty(uint8_t card)
{
	if (!m_manager) return;
	CChunkPart* chunkpart = 0x0;
	int cx = m_position.x;
	int cz = m_position.z;
	int cy = m_position.y; 
	switch (card)
	{
	case CARD_S: //x==0
		chunkpart = m_manager->getChunkPart(cx - 1, cz);
		break;
	case CARD_N://x==16
		chunkpart = m_manager->getChunkPart(cx + 1, cz);
		break;
	case CARD_W://z==0
		chunkpart = m_manager->getChunkPart(cx, cz - 1);
		break;
	case CARD_E://z==16
		chunkpart = m_manager->getChunkPart(cx, cz + 1);
		break;
	default:
		chunkpart = m_manager->getChunkPart(cx, cz);
		break;
	}
	if (chunkpart)
	{
		CChunk* chunk = 0x0;
		if (card == CARD_UP)
			chunk = chunkpart->getChunk(cy + 1);
		else if(card == CARD_DOWN)
			chunk = chunkpart->getChunk(cy - 1);
		else
			chunk = chunkpart->getChunk(cy);

		if (chunk)
			chunk->getMeshComponent().makeDirty();
	}
}

void CChunk::makeNeighborsDirty()
{
	if (!m_manager) return;

	int cx = m_position.x;
	int cz = m_position.z;
	int cy = m_position.y;
	std::vector<CChunk*> m_neighbours;
	CChunkPart* chunkpart[] = {
		m_manager->getChunkPart(cx - 1, cz),
		m_manager->getChunkPart(cx + 1, cz),
		m_manager->getChunkPart(cx, cz - 1),
		m_manager->getChunkPart(cx, cz + 1)
	};
	for(int i = 3; i >= 0; --i) 
	{
		if(chunkpart[i])
		{
			CChunk* chunk = chunkpart[i]->getChunk(cy);
			m_neighbours.push_back(chunk);
		}
	}
	CChunkPart* parent = m_manager->getChunkPart(cx, cz);
	if(parent)
	{
		m_neighbours.push_back(parent->getChunk(cy + 1));
		m_neighbours.push_back(parent->getChunk(cy - 1));
	}
	for(CChunk* chunk : m_neighbours)
		if(chunk) chunk->getMeshComponent().makeDirty();
}
