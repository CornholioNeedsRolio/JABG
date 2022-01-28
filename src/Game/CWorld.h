#ifndef CWORLD_HEADER
#define CWORLD_HEADER
#include "../CObject.h"
#include "CChunkPart.h"
#include "CTerrainGenerator.h"
#include "CChunkMeshThreadManager.h"
#include "CChunkManager.h"
#include "../Misc/CFPPlayer.h"
#include "ChunkLoader/CChunkLoader.h"

#include <glm/glm.hpp>


enum
{
    SET_INVALIDCHUNK,
    SET_INVALIDPART,
    SET_THREADRUNNING,
    SET_OUTOFBOUNDS,
    SET_SAMETYPE,
    SET_SUCCESFUL
};

class CWorld : public CObject
{
    CChunkManager m_manager;
    CChunkMeshThreadManager m_threadManager;
    CChunkLoader m_loader;


    std::tuple<int, int, int> m_playerPos;

    std::shared_ptr<CTextureAtlas> m_texture = 0x0;
    std::shared_ptr<CShader> m_shader = 0x0;
    std::string m_filePath = "./world/";
    CFPPlayer* m_player = nullptr;

    char m_flags;
    enum
    {
        NOT_FIRST_RUN = 1
    };
public:
    CWorld(std::shared_ptr<CTextureAtlas> texture, CFPPlayer& player);
    ~CWorld();

    void setBlock(int x, int y, int z, int id);
    int getBlock(int x, int y, int z);

    CChunkManager& getManager();
    const std::string& getFilePath() const;

    int getDrawDistance();
    std::shared_ptr<CTextureAtlas> getAtlas();
    std::shared_ptr<CShader> getShader();

    void Draw(const SDrawInfo& info) override;
    void BulkDraw(class CBulkRenderer* renderer) override;
    void Tick(CInputManager& _manager, float deltaTime) override;
};

#endif