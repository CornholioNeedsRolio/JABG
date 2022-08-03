#ifndef CChunkSaveComponent_Header
#define CChunkSaveComponent_Header
#include <fstream>
#include <memory>
#include <array>
#include <vector>

#define MEGA_CHUNK_SIZE 16

typedef short FileChunkPointer;
struct SMegaChunkFile
{
            std::array<FileChunkPointer, MEGA_CHUNK_SIZE*MEGA_CHUNK_SIZE*MEGA_CHUNK_SIZE> ChunkPointer{-1};
            struct SChunkInfo
            {
                        bool isInit = false;
                        std::unique_ptr<struct SBlockInfo[]> BlockInfos;
            };
            std::array<SChunkInfo, MEGA_CHUNK_SIZE*MEGA_CHUNK_SIZE*MEGA_CHUNK_SIZE> ChunkInfos;
};

class CChunkSaveComponent
{
    class CChunk* m_parent = nullptr;
    std::string getChunkPath(const std::string filePath) const;
    FileChunkPointer getChunkOffset() const;

    SMegaChunkFile LoadFile(const std::string& path, bool& isok, FileChunkPointer SpecificChunk = -1) const;
    void SaveFile(const std::string& path, SMegaChunkFile& chunkFile);

    std::tuple<int, int, int> GetMegaChunkPos() const;
    std::tuple<int, int, int> GetChunkLocalPos() const;

    static std::vector<SMegaChunkFile> Cache;
public:
    CChunkSaveComponent(class CChunk* parent);

    void save(const std::string& path); //makes file
    bool load(const std::string& path); //returns false if it failed loading, and true if it worked
};

#endif