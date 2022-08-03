#include "CChunkSaveComponent.h"
#include <filesystem>
#include "../CChunk.h"
#include <fstream>
#include <string>

CChunkSaveComponent::CChunkSaveComponent(CChunk* parent) : m_parent(parent)
{

}

std::tuple<int, int, int> CChunkSaveComponent::GetMegaChunkPos() const
{
            auto& position = m_parent->getChunkPosition();
            int cx = std::floor((float)position.x / (float)MEGA_CHUNK_SIZE);
            int cy = std::floor((float)position.y / (float)MEGA_CHUNK_SIZE);
            int cz = std::floor((float)position.z / (float)MEGA_CHUNK_SIZE);

            return std::make_tuple(cx, cy, cz);
}

std::tuple<int, int, int> CChunkSaveComponent::GetChunkLocalPos() const
{
            auto& position = m_parent->getChunkPosition();
            int bx = position.x % MEGA_CHUNK_SIZE;
            if (bx < 0) bx += MEGA_CHUNK_SIZE;
            int by = position.y % MEGA_CHUNK_SIZE;
            if (by < 0) by += MEGA_CHUNK_SIZE;
            int bz = position.z % MEGA_CHUNK_SIZE;
            if (bz < 0) bz += MEGA_CHUNK_SIZE;

            return std::make_tuple(bx, by, bz);
}

std::string CChunkSaveComponent::getChunkPath(const std::string filePath) const
{
            auto position = GetMegaChunkPos();
            return filePath + std::to_string(std::get<0>(position))+"r"+std::to_string(std::get<1>(position))+"r"+ std::to_string(std::get<2>(position))+".mc"; //.mc mega chunk
}

FileChunkPointer CChunkSaveComponent::getChunkOffset() const
{
            auto position = GetChunkLocalPos();
            return std::get<0>(position) + MEGA_CHUNK_SIZE*(std::get<1>(position)+std::get<2>(position)*MEGA_CHUNK_SIZE);
}

void CChunkSaveComponent::save(const std::string& path)
{
            bool isOk = false; FileChunkPointer chunkPos = getChunkOffset();
            SMegaChunkFile file = LoadFile(path, isOk);
            SBlockInfo* infos = m_parent->getVoxelComponent().getAllBlocks();
            file.ChunkInfos[chunkPos].BlockInfos = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
            file.ChunkInfos[chunkPos].isInit = true;
            for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; ++i )
                        file.ChunkInfos[chunkPos].BlockInfos[i] = infos[i];
            SaveFile(path, file);
}

bool CChunkSaveComponent::load(const std::string& path)
{
            bool isOk = false; short chunkPos = getChunkOffset();
            SMegaChunkFile file = LoadFile(path, isOk,chunkPos);
            if(isOk) {
                        if(!file.ChunkInfos[chunkPos].isInit) return false;
                        m_parent->getVoxelComponent().setAllBlocks(std::move(file.ChunkInfos[chunkPos].BlockInfos));
                        return true;
            }
            return false;
}

SMegaChunkFile CChunkSaveComponent::LoadFile(const std::string& path, bool& isok, FileChunkPointer SpecificChunk) const
{
            SMegaChunkFile output;
            isok = false;
            //reading file and halting if it fails
            if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
                        return output;
            }
            std::string chunk_path = getChunkPath(path);                                    //get the path of the chunk
            std::ifstream file(chunk_path, std::ios::binary | std::ios::in);            //open the mega chunk
            if(!file.good() || !file.is_open())
                        return output;
            //Reading the chunk pointers
            file.read((char*)&output.ChunkPointer[0], CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*sizeof(FileChunkPointer));
            //Loading chunks into the struct
            auto LoadChunk = [&](FileChunkPointer chunk)
            {
                        short ChunkPointer = output.ChunkPointer[chunk]; //if we want a specific chunk
                        if(ChunkPointer == -1) return;
                        file.seekg(ChunkPointer-1);
                        output.ChunkInfos[chunk].BlockInfos = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
                        output.ChunkInfos[chunk].isInit = true;
                        file.read((char*)&output.ChunkInfos[chunk].BlockInfos[0], CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*sizeof(SBlockInfo));
            };

            if(SpecificChunk == -1) {
                        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++i)
                                    LoadChunk(i);
            }else{
                        LoadChunk(SpecificChunk);
            }
            //done loading
            isok = true;
            return output;
}

void CChunkSaveComponent::SaveFile(const std::string &path, SMegaChunkFile& chunkFile)
{
            if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
                        std::filesystem::create_directory(path);
            }
            std::string chunk_path = getChunkPath(path);
            std::ofstream file(chunk_path, std::ios::binary | std::ofstream::trunc | std::ios::out);
            std::vector<char> FileBuffer;
            for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; ++i )
            {
                        if(!chunkFile.ChunkInfos[i].isInit) continue;
                        std::vector<char> buffer{};
                        size_t size = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*sizeof(SBlockInfo);
                        buffer.reserve(size);
                        memcpy(&buffer[0], &chunkFile.ChunkInfos[i], size);
                        chunkFile.ChunkPointer[i] = FileBuffer.size();

                        FileBuffer.insert(FileBuffer.end(), buffer.begin(), buffer.end());
            }

            file.write((char*)&chunkFile.ChunkInfos[0], sizeof(chunkFile.ChunkInfos));
            file.write((char*)&FileBuffer[0], FileBuffer.size());

            file.close();
}
