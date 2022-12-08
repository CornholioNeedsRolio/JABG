#ifndef JABG_CMEGACHUNK_H
#define JABG_CMEGACHUNK_H
#include <glm/glm.hpp>
#include "Game/Chunk/CChunk.h"

#define MEGA_CHUNK_SIZE 32
#define MEGA_CHUNK_SIZE_SQR MEGA_CHUNK_SIZE*MEGA_CHUNK_SIZE*MEGA_CHUNK_SIZE

class CMegaChunk
{
	float m_timeExisted = 0;
	glm::ivec3 m_Pos;

	std::array<std::uint32_t, MEGA_CHUNK_SIZE_SQR> m_chunkIndexes{ 0 };
	std::array<std::pair<bool, std::vector<char>>, MEGA_CHUNK_SIZE_SQR> m_serializedChunks; // first = initialized, vector of chars the chunk

	std::string GetFileName() const;

	std::vector<char> SerializeMegaChunk();
	void DeserializeMegaChunk(const std::vector<char>& serialized);

	std::vector<char> Decompress(const std::vector<char>& Compressed);
	std::vector<char> Compress(const std::vector<char>& Decompressed);

	class CWorld* m_world = nullptr;
public:
	CMegaChunk(int x, int y, int z, CWorld* world);

	void AddChunk(int x, int y, int z, const std::vector<char>& serializedData);
	bool GetChunk(int x, int y, int z, std::vector<char>& Data);

	void Save();
	void Load();

	bool PositionEqual(int x, int y, int z);
	bool AddTime(float DeltaTime);
	void ResetTime();

};


#endif //JABG_CMEGACHUNK_H
