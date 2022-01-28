#ifndef CChunkMeshComponent_Header
#define CChunkMeshComponent_Header
#include <atomic>
#include <mutex>
#include "../../CMesh.h"

class CChunkMeshComponent
{
    CMesh m_mesh;
    class CChunk* m_parent = nullptr;
    std::atomic_char m_flags = 0;
    std::atomic_int m_currentDirty = 0, m_lastDirty = 0;
    std::mutex m_mutex;
    std::vector<SVertex> m_meshinfo;
    enum
    {
        THREAD_RUNNING = 0x1,
        THREAD_DONE = 0x2
    };
public:
    CChunkMeshComponent(CChunk* parent);
    ~CChunkMeshComponent();

    void Draw(class CBulkRenderer* renderer);
    void setTextureAndShader(std::shared_ptr<class CTextureAtlas> texture, std::shared_ptr<class CShader> shader);
    void BuildMeshData(std::array<CChunk*, 6> neighbors, std::shared_ptr<class CTextureAtlas> atlas);
    void makeDirty();
    bool isDirty() const;
};

#endif