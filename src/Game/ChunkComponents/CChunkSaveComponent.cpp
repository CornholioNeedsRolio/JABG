#include "CChunkSaveComponent.h"
#include <filesystem>
#include "../CChunk.h"
#include <fstream>
#include <string>

CChunkSaveComponent::CChunkSaveComponent(CChunk* parent) : m_parent(parent)
{
		const glm::ivec3& pos = m_parent->getChunkPosition();
		int cx = std::floor((float)pos.x / (float)MEGA_CHUNK_SIZE);
		int cy = std::floor((float)pos.y / (float)MEGA_CHUNK_SIZE);
		int cz = std::floor((float)pos.z / (float)MEGA_CHUNK_SIZE);
		int bx = pos.x % MEGA_CHUNK_SIZE;
		if (bx < 0) bx += MEGA_CHUNK_SIZE;
		int by = pos.y % MEGA_CHUNK_SIZE;
		if (by < 0) by += MEGA_CHUNK_SIZE;
		int bz = pos.z % MEGA_CHUNK_SIZE;
		if (bz < 0) bz += MEGA_CHUNK_SIZE;
		m_megaChunkPos = {cx, cy, cz};
		m_localChunkPos = {bx, by, bz};
}

void CChunkSaveComponent::save()
{

}

bool CChunkSaveComponent::load()
{
			return false;
}

const std::tuple<int, int, int>& CChunkSaveComponent::GetMegaChunkPos() const
{
			return m_megaChunkPos;
}

const std::tuple<int, int, int>& CChunkSaveComponent::GetLocalChunkPos() const
{
			return m_localChunkPos;
}

CChunk *CChunkSaveComponent::getParent() const
{
			return m_parent;
}
