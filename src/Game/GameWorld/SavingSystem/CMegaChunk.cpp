#include <filesystem>
#include <iostream>
#include <zlib.h>
#include "CMegaChunk.h"
#include "Game/GameWorld/CWorld.h"

CMegaChunk::CMegaChunk(int x, int y, int z, CWorld* world) : m_Pos(x, y, z), m_world(world)
{
			for(auto& serializedChunk : m_serializedChunks)
						serializedChunk.first = false;
}

std::vector<char> CMegaChunk::Decompress(const std::vector<char>& Compressed)
{
			std::vector<char> Decompressed;
			//getting the size of the archive
			std::uint32_t size;
			memcpy(&size, Compressed.data(), sizeof size);

			//doing the decompression
			uLong u_size = size;
			Decompressed.resize(size);
			//the pointer of compressed is offseted by the size since it's not part of the compression
			uncompress((Bytef*)Decompressed.data(), &u_size, (Bytef*)(Compressed.data() + sizeof(size)), Compressed.size() - sizeof size);
			return Decompressed;
}

std::vector<char> CMegaChunk::Compress(const std::vector<char>& Decompressed)
{
			std::vector<char> Compressed;

			//making compressed the same size as decompressed
			Compressed.resize(compressBound(Decompressed.size()));
			std::uint32_t size = Compressed.size();
			uLong u_size = size;

			//compressing data to Compressed
			compress((Bytef*)Compressed.data(), &u_size, (Bytef*)Decompressed.data(), Decompressed.size());
			//resizing Compressed to the new size
			Compressed.resize(u_size);

			//Serializing size and adding it to the beginning
			size = Decompressed.size();
			char SerializedSize[sizeof size];
			memcpy(&SerializedSize[0], &size, sizeof size);
			Compressed.insert(Compressed.begin(), &SerializedSize[0], &SerializedSize[0]+sizeof size);

			return Compressed;
}

void CMegaChunk::Load()
{
			std::string path = m_world->getFilePath();
			if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
						return;
			}
			std::ifstream file(path+GetFileName());
			if(!file.good() || !file.is_open()) {
						return;
			}

			file.seekg(0, std::ios::end);
			size_t fileSize = file.tellg();
			file.seekg(0, std::ios::beg);

			std::vector<char> Ser_Buffer;
			Ser_Buffer.resize(fileSize);
			file.read(Ser_Buffer.data(), Ser_Buffer.size());
			std::vector<char> Ser_Buffer_Decomp = Decompress(Ser_Buffer);
			DeserializeMegaChunk(Ser_Buffer_Decomp);
			file.close();
}

void CMegaChunk::Save()
{
			std::string path = m_world->getFilePath();
			if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
						std::filesystem::create_directory(path);
			}

			std::ofstream file(path+GetFileName(), std::ios::trunc);
			if(!file.good() || !file.is_open()) {
						return;
			}

			std::vector<char> Ser_Buffer = SerializeMegaChunk();
			std::vector<char> SerBuffer_Comp = Compress(Ser_Buffer);
			file.write(SerBuffer_Comp.data(), SerBuffer_Comp.size());
			//file.write(Ser_Buffer.data(), Ser_Buffer.size());
			file.close();
}

std::vector<char> CMegaChunk::SerializeMegaChunk()
{
			std::vector<char> Ser_Output{},Ser_ChunkOffset{},Ser_ChunkData{};
			//reseting all offsets to 0
			memset(&m_chunkIndexes[0], 0, sizeof(m_chunkIndexes));
			for(int i = 0; i < MEGA_CHUNK_SIZE_SQR; ++i)
			{
						if(!m_serializedChunks[i].first) continue;
						std::vector<char> Ser_TempChunkData{};
						unsigned short ChunkSize = m_serializedChunks[i].second.size();
						Ser_TempChunkData.reserve(sizeof ChunkSize);
						for(int j = 0; j < sizeof(ChunkSize); ++j)
									Ser_TempChunkData.push_back(*(((char*)&ChunkSize)+j));
						Ser_TempChunkData.insert(Ser_TempChunkData.end(), m_serializedChunks[i].second.begin(), m_serializedChunks[i].second.end());
						m_chunkIndexes[i] = sizeof(m_chunkIndexes) + Ser_ChunkData.size();
						Ser_ChunkData.insert(Ser_ChunkData.end(), Ser_TempChunkData.begin(), Ser_TempChunkData.end());
			}
			Ser_ChunkOffset.resize(sizeof(m_chunkIndexes));
			memcpy(Ser_ChunkOffset.data(), &m_chunkIndexes[0], sizeof(m_chunkIndexes));
			//merging the 2 parts of the serialization
			Ser_Output.insert(Ser_Output.end(), Ser_ChunkOffset.begin(), Ser_ChunkOffset.end());
			Ser_Output.insert(Ser_Output.end(), Ser_ChunkData.begin(), Ser_ChunkData.end());
			return Ser_Output;
}

void CMegaChunk::DeserializeMegaChunk(const std::vector<char>& serialized)
{
			memcpy(&m_chunkIndexes[0], serialized.data(), sizeof(m_chunkIndexes));
			for(int i = 0; i < MEGA_CHUNK_SIZE_SQR; ++i)
			{
						if(m_chunkIndexes[i] == 0) continue;
						unsigned short ChunkSize;
						memcpy(&ChunkSize, &serialized[m_chunkIndexes[i]] , sizeof(ChunkSize));
						m_serializedChunks[i].first = true;
						m_serializedChunks[i].second.resize(ChunkSize);
						memcpy(m_serializedChunks[i].second.data(), &serialized[m_chunkIndexes[i] + sizeof(ChunkSize)], ChunkSize);
			}
}

void  CMegaChunk::AddChunk(int x, int y, int z, const std::vector<char>& serializedData)
{
			const int index = x + MEGA_CHUNK_SIZE*(y + z * MEGA_CHUNK_SIZE);
			m_serializedChunks[index].first = true;
			m_serializedChunks[index].second = serializedData;
}

bool CMegaChunk::GetChunk(int x, int y, int z, std::vector<char> &Data)
{
			const int index = x + MEGA_CHUNK_SIZE*(y + z * MEGA_CHUNK_SIZE);
			if(!m_serializedChunks[index].first) return false;
			Data = m_serializedChunks[index].second;
			return true;
}

std::string CMegaChunk::GetFileName() const
{
			return
					std::to_string(  m_Pos.x)+'.'
					+std::to_string(m_Pos.y)+'.'
					+std::to_string(m_Pos.z)+".c";
}

bool CMegaChunk::PositionEqual(int x, int y, int z)
{
			return m_Pos == glm::ivec3(x, y, z);
}

bool CMegaChunk::AddTime(float DeltaTime)
{
			m_timeExisted += DeltaTime;
			return m_timeExisted >= 48.f;
}

void CMegaChunk::ResetTime()
{
			m_timeExisted = 0;
}
