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
			std::string getChunkPath(const std::string filePath) const;
public:
			explicit CChunkSaveComponent(class CChunk* parent);

			void save(const std::string& path);
			bool load(const std::string& path);
};

#endif