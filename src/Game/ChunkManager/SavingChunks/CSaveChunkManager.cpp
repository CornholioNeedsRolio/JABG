//
// Created by rellik on 8/3/22.
//

#include "CSaveChunkManager.h"
#include "../CChunkManager.h"
#include "../../CWorld.h"
#include <filesystem>
#include <iostream>

CSaveChunkManager::CSaveChunkManager(CChunkManager* parent, std::tuple<int, int, int> MegaChunkPos) : m_parent(parent), m_megaChunkPos(MegaChunkPos)
{
	memset(&FileOffsets, 0, sizeof FileOffsets);
	m_timeExisted = 0;
	LoadFile();
}

CSaveChunkManager::~CSaveChunkManager()
{
	SaveFile();
}

std::vector<char> CSaveChunkManager::TurnToByteStr()
{
		std::vector<char> ChunkBuffers{};
		//finding the offsets, and turning chunks to a char array
		for(int i = 0; i < LoadedChunks.size(); ++i)
		{
				if(!LoadedChunks[i].BlockData) {
							FileOffsets[i] = 0;
							continue;
				}
				std::vector<char> CurrentBuffer{};
				CurrentBuffer.reserve(LoadedChunks[i].getSize());
				//serializing to byte
				memcpy(CurrentBuffer.data(), LoadedChunks[i].BlockData.get(), LoadedChunks[i].getSize());

				//where this is going to be in the file
				FileOffsets[i] = sizeof(FileOffsets) + ChunkBuffers.size();
				ChunkBuffers.insert(ChunkBuffers.end(), CurrentBuffer.begin(), CurrentBuffer.end());
		}
		//adding the offsets to the char file
		std::vector<char> CurrentBuffer{};
		CurrentBuffer.reserve(sizeof(FileOffsets));
		memcpy(CurrentBuffer.data(), &FileOffsets[0], sizeof(FileOffsets));
		ChunkBuffers.insert(ChunkBuffers.begin(), CurrentBuffer.begin(), CurrentBuffer.end());
		//returning the stuff we processed
		return ChunkBuffers;
}

void CSaveChunkManager::FromByteStr(const std::vector<char>& ByteStr)
{
		if(ByteStr.size() < sizeof(FileOffsets)) return;
		memcpy(&FileOffsets[0], ByteStr.data(), sizeof(FileOffsets));
		for(int i = 0; i < FileOffsets.size(); ++i)
		{
					if(!FileOffsets[i]) continue;
					LoadedChunks[i].Init();
					memcpy(&LoadedChunks[i].BlockData[0], &ByteStr[FileOffsets[i]], sizeof(LoadedChunks[i].getSize()));
		}
}

void CSaveChunkManager::LoadFile()
{
			std::string path = m_parent->getWorld()->getFilePath();
			if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
						return;
			}
			std::string chunk_path = GetFilePath();
			std::ifstream file(chunk_path, std::ios::binary | std::ios::in);
			if(!file.good()) return;
			std::vector<char> buffer;

			//getting file size
			file.seekg(0, std::ios::end);
			size_t fileSize = file.tellg();
			std::cerr << fileSize << std::endl;
			file.seekg(0, std::ios::beg);
			//copying the file to the bugger
			buffer.reserve(fileSize);
			file.read(buffer.data(), fileSize);

			FromByteStr(buffer);

			file.close();
}

void CSaveChunkManager::SaveFile()
{
			std::string path = m_parent->getWorld()->getFilePath();
			if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
						std::filesystem::create_directory(path);
			}

			std::string chunk_path = GetFilePath();
			std::ofstream file(chunk_path, std::ios::binary | std::ofstream::trunc | std::ios::out);
			if(!file.good()) return;
			std::vector<char> buffer = TurnToByteStr();
			file.write(buffer.data(), buffer.size());
			file.close();
}

bool CSaveChunkManager::Load(CChunkSaveComponent* saveComponent)
{
	std::lock_guard g(m_mutex);
	const int index = saveComponent->GetLocalChunkIndex();

	if(!LoadedChunks[index].BlockData) return false;
	std::unique_ptr<SBlockInfo[]> temp = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);

	std::copy(LoadedChunks[index].BlockData.get(), LoadedChunks[index].BlockData.get() + CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE, temp.get());
	saveComponent->getParent()->getVoxelComponent().setAllBlocks(std::move(temp));
	return true;
}

void CSaveChunkManager::Save(CChunkSaveComponent* saveComponent)
{
			std::lock_guard g(m_mutex);
			const int index = saveComponent->GetLocalChunkIndex();
			LoadedChunks[index].BlockData = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
			SBlockInfo* block = saveComponent->getParent()->getVoxelComponent().getAllBlocks();
			std::copy(block, block + CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE, LoadedChunks[index].BlockData.get());
}

std::string CSaveChunkManager::GetFilePath() const
{
			return m_parent->getWorld()->getFilePath()
			+std::to_string(std::get<0>(m_megaChunkPos))+'.'
			+std::to_string(std::get<1>(m_megaChunkPos))+'.'
			+std::to_string(std::get<2>(m_megaChunkPos))+".c";
}

float CSaveChunkManager::IncrementTime(float deltaTime)
{
			return (m_timeExisted += deltaTime);
}

void CSaveChunkManager::ChunkData::Init()
{
			BlockData = std::make_unique<SBlockInfo[]>(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE);
}

const std::tuple<int, int, int>& CSaveChunkManager::GetMegaChunkPos() const
{
			return m_megaChunkPos;
}