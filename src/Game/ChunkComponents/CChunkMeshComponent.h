#ifndef CChunkMeshComponent_Header
#define CChunkMeshComponent_Header
#include <atomic>
#include <mutex>
#include "Engine/CMesh.h"
#include "IChunkComponent.h"

struct STextureAndShader
{
    std::shared_ptr<class CTextureAtlas> texture;
    std::shared_ptr<class CShader> shader;
};

class CChunkMeshComponent : public IChunkComponent
{
    class CChunk* m_parent = nullptr;
    std::atomic_char m_flags = 0;
    std::atomic_int m_currentDirty = 0, m_lastDirty = 0;
    std::mutex m_mutex;

    std::vector<SVertex> m_meshinfo, m_watermeshinfo, m_glassmeshinfo;
    CMesh m_mesh, m_watermesh, m_glassmesh;
    enum
    {
        THREAD_RUNNING = 0x1,
        THREAD_DONE = 0x2
    };
public:
    CChunkMeshComponent(CChunk* parent);
    ~CChunkMeshComponent();

    void Draw(class CBulkRenderer* renderer);
    void setTextureAndShader(STextureAndShader normal, STextureAndShader water);
    void BuildMeshData(std::array<CChunk*, 27> neighbors, std::shared_ptr<class CTextureAtlas> atlas);
    void MakeDirty() override;
    bool IsDirty() const override;
};

#endif