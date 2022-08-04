#ifndef JABG_CSAVECHUNKMANAGER_H
#define JABG_CSAVECHUNKMANAGER_H
#include "../../CChunk.h"

typedef unsigned int ChunkFileOffset;
class CSaveChunkManager
{
			//if offset is 0, then it's null, this shouldn't be any value under 0x4000 , 4096*4(I hate the fact that it's compiler dependent if the size of int is going to be 3 or 4
			std::array<ChunkFileOffset, CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE> FileOffsets;
			//decompiled file data
			struct ChunkData
			{
						std::unique_ptr<SBlockInfo[]> BlockData = nullptr;
						void Init();
						size_t getSize() const
						{
									return CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*sizeof(SBlockInfo);
						}
			};
			//all the chunks loaded, if BlockData is null it's invalid
			std::array<ChunkData, CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE> LoadedChunks;

			float m_timeExisted;
			class CChunkManager* m_parent = nullptr;
			std::tuple<int, int, int> m_megaChunkPos;

			std::vector<char> TurnToByteStr();
			void FromByteStr(const std::vector<char>& ByteStr);

			std::string GetFilePath() const;
public:
			CSaveChunkManager(CChunkManager* parent, std::tuple<int, int, int> MegaChunkPos);
			float IncrementTime(float deltaTime);

			void LoadFile();
			void SaveFile();

			void Load(class CChunkSaveComponent* saveComponent);
			bool Save(class CChunkSaveComponent* saveComponent);
};


#endif //JABG_CSAVECHUNKMANAGER_H
