#ifndef CChunkSaveComponent_Header
#define CChunkSaveComponent_Header
#include <fstream>
#include <memory>
#include <array>
#include <vector>

#define MEGA_CHUNK_SIZE 16


class CChunkSaveComponent
{
			class CChunk* m_parent = nullptr;
			void old_save();
			bool old_load();

			void new_save();
			void new_load();

			std::tuple<int, int, int> m_megaChunkPos, m_localChunkPos;
			std::string getChunkPath(const std::string& filePath) const;
public:
			CChunkSaveComponent(class CChunk* parent);
			const std::tuple<int, int, int>& GetMegaChunkPos() const;
			const std::tuple<int, int, int>& GetLocalChunkPos() const;
			int GetLocalChunkIndex() const;
			CChunk *getParent() const;

			void save();
			bool load();
};

#endif