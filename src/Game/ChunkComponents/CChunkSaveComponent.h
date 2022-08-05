#ifndef CChunkSaveComponent_Header
#define CChunkSaveComponent_Header
#include <fstream>
#include <memory>
#include <array>
#include <vector>

#define MEGA_CHUNK_SIZE 16
//#define DONT_LOAD_CHUNKS


class CChunkSaveComponent
{
			class CChunk* m_parent = nullptr;

			std::tuple<int, int, int> m_megaChunkPos, m_localChunkPos;
public:
			CChunkSaveComponent(class CChunk* parent);
			const std::tuple<int, int, int>& GetMegaChunkPos() const;
			const std::tuple<int, int, int>& GetLocalChunkPos() const;
			CChunk *getParent() const;

			void save();
			bool load();

};

#endif