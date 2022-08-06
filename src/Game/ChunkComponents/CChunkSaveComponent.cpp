#include "Game/ChunkManager/CChunkManager.h"
#include "Game/CWorld.h"
#include "CChunkSaveComponent.h"
#include <filesystem>
#include "../CChunk.h"
#include <fstream>
#include <string>

#define OLD_LOAD

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

std::string CChunkSaveComponent::getChunkPath(const std::string& filePath) const
{
			glm::ivec3 chunkPos = m_parent->getChunkPosition();
			return filePath + std::to_string(chunkPos.x)+'.'+std::to_string(chunkPos.y)+'.'+ std::to_string(chunkPos.z)+".sc";
}

void CChunkSaveComponent::old_save()
{
			std::string path = m_parent->getManager()->getWorld()->getFilePath();
			if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
						std::filesystem::create_directory(path);
			}
			std::string chunk_path = getChunkPath(path);
			std::ofstream file(chunk_path, std::ios::binary | std::ofstream::trunc | std::ios::out);
			SBlockInfo* blocks = m_parent->getVoxelComponent().getAllBlocks();
			if(!file.good() || blocks == nullptr) return;
			for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; ++i)
						file.write((char*)&blocks[i], sizeof(SBlockInfo));
			file.close();
}

bool CChunkSaveComponent::old_load()
{
			std::string path = m_parent->getManager()->getWorld()->getFilePath();
			if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
						return false;
			}

			std::string chunk_path = getChunkPath(path);
			std::ifstream file(chunk_path, std::ios::binary | std::ios::in);
			if(!file.good() || !file.is_open())
						return false;
			std::unique_ptr<SBlockInfo[]> output = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
			for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; ++i)
						file.read((char*)&output[i], sizeof(SBlockInfo));
			m_parent->getVoxelComponent().setAllBlocks(std::move(output));
			file.close();
			return true;
}

void CChunkSaveComponent::new_save()
{

}

void CChunkSaveComponent::new_load()
{

}

void CChunkSaveComponent::save()
{
#ifdef OLD_LOAD
	old_save();
#else
	new_save();
#endif
}

bool CChunkSaveComponent::load()
{
#ifdef OLD_LOAD
			return old_load();
#else
			return new_load();
#endif
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

int CChunkSaveComponent::GetLocalChunkIndex() const {
			return std::get<0>(m_localChunkPos) + MEGA_CHUNK_SIZE*(std::get<1>(m_localChunkPos) + std::get<2>(m_localChunkPos)*MEGA_CHUNK_SIZE);
}
