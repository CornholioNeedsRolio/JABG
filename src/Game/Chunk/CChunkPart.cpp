#include "ChunkComponents/CChunkMeshComponent.h"
#include "CChunkPart.h"
#include "Game/GameWorld/CWorld.h"
#include <array>
#include <iostream>
#include <cmath>

CChunkPart::CChunkPart(CWorld* world, glm::ivec2 position) : 
	 m_world(world), m_position(position), m_chunks{}
{
	m_normalHeightMap = std::make_unique<CLightHeightMap>();
}

CChunk* CChunkPart::getChunk(int y)
{
	if (m_chunks.count(y))
		return m_chunks[y].get();
	return nullptr;
}

void CChunkPart::UnloadChunks(int cy, int halfRenderDistance)
{
	for(auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		if(std::abs(it->second->getChunkPosition().y - cy) > halfRenderDistance)
			it = m_chunks.erase(it);
		else ++it;
	}
}

CChunk* CChunkPart::getChunkNew(int y)
{
	CChunk* chunk = getChunk(y);
	if(!chunk) 
		m_chunks.emplace(y, std::shared_ptr<CChunk>(new CChunk(glm::ivec3(m_position.x, y, m_position.y), &m_world->getManager())));
	return m_chunks[y].get();
}

CChunk* CChunkPart::getChunkInRange(int closeToY, bool fuckingClean, int* pos)
{
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++)
		if (it->second->getMeshComponent().IsDirty())
		{
			if (pos) *pos = it->first;
			return it->second.get();
		}

	return nullptr;
}

CChunk* CChunkPart::addChunk(const std::shared_ptr<CChunk>& chunk)
{
	chunk->setManager(&m_world->getManager(), this);
	m_chunks.insert(std::make_pair(chunk->getChunkPosition().y, chunk));
	return chunk.get();
}

void CChunkPart::MakeDirty()
{
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++)
		it->second->getMeshComponent().MakeDirty();
}

bool CChunkPart::destroyable()
{
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++)
		return false;
	return true;
}

void CChunkPart::Draw(const SDrawInfo& info){}

void CChunkPart::BulkDraw(class CBulkRenderer* renderer)
{
	for (auto it = m_chunks.begin(); it != m_chunks.end(); ++it)
	{
		it->second->getMeshComponent().setTextureAndShader({m_world->getAtlas(), m_world->getShader()}, {m_world->getAtlas(), m_world->getWaterShader()});
		it->second->BulkDraw(renderer);
	}
}

CLightHeightMap* CChunkPart::getNormalMap() const
{
	return m_normalHeightMap.get();
}

std::unordered_map<int, std::shared_ptr<CChunk>>::iterator CChunkPart::getBeginIterator()
{
	return m_chunks.begin();
}

std::unordered_map<int, std::shared_ptr<CChunk>>::iterator CChunkPart::getEndIterator()
{
	return m_chunks.end();
}

glm::ivec2& CChunkPart::getChunkPartPosition()
{
	return m_position;
}
