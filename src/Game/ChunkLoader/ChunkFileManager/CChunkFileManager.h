#ifndef CChunkFileManager_Header
#define CChunkFileManager_Header
#include <vector>

class CChunkFileManager
{
    std::vector<class CChunk*> m_pendingChunks;
    std::string m_path;
public:
    CChunkFileManager(const std::string& path);
}

#endif