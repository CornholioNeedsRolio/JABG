#ifndef CChunkSaveComponent_Header
#define CChunkSaveComponent_Header
#include <fstream>
#include <memory>

class CChunkSaveComponent
{
    class CChunk* m_parent = nullptr;
    std::string getChunkPath(const std::string filePath) const;
public:
    CChunkSaveComponent(class CChunk* parent);

    void save(const std::string& path);
    bool load(const std::string& path);
};

#endif