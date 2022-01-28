#include "CChunkSaveComponent.h"
#include <filesystem>
#include "../CChunk.h"
#include <fstream>
#include <string>

CChunkSaveComponent::CChunkSaveComponent(CChunk* parent) : m_parent(parent)
{

}

std::string CChunkSaveComponent::getChunkPath(const std::string filePath) const
{
    glm::ivec3 chunkPos = m_parent->getChunkPosition();
    return filePath + std::to_string(chunkPos.x)+'m'+std::to_string(chunkPos.y)+'m'+ std::to_string(chunkPos.z)+"chunk";
}

void CChunkSaveComponent::save(const std::string& path)
{
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

bool CChunkSaveComponent::load(const std::string& path)
{
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