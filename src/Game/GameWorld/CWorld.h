#ifndef CWORLD_HEADER
#define CWORLD_HEADER
#include "Engine/Objects/CObject.h"
#include "Game/Chunk/CChunkPart.h"
#include "Game/CTerrainGenerator.h"
#include "Game/CChunkMeshThreadManager.h"
#include "Game/Chunk/ChunkManager/CChunkManager.h"
#include "Misc/CFPPlayer.h"
#include "Game/Chunk/ChunkLoader/CChunkLoader.h"

#include "glm/glm.hpp"
#include "Game/GameWorld/SavingSystem/CMegaChunkManager.h"


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
    CChunkManager* m_manager;
    CChunkMeshThreadManager m_threadManager;
    CChunkLoader m_loader;
	CMegaChunkManager m_loadingComponent;

    std::tuple<int, int, int> m_playerPos;

    std::shared_ptr<CTextureAtlas> m_texture = 0x0;
    std::shared_ptr<CShader> m_shader = 0x0, m_watershader = 0x0;
    std::string m_filePath = "./world/";
    CFPPlayer* m_player = nullptr;

    char m_flags;
    enum
    {
        NOT_FIRST_RUN = 1
    };
public:
    CWorld(std::shared_ptr<CTextureAtlas> texture, CFPPlayer& player);
    ~CWorld() override;

    void setBlock(int x, int y, int z, int id);
    int getBlock(int x, int y, int z);

    CChunkManager& getManager();
    const std::string& getFilePath() const;

	CMegaChunkManager& GetLoader();

    int getDrawDistance();
    std::shared_ptr<CTextureAtlas> getAtlas();
    std::shared_ptr<CShader> getShader();
    std::shared_ptr<CShader> getWaterShader();

    void Draw(const SDrawInfo& info) override;
    void BulkDraw(class CBulkRenderer* renderer) override;
    void Tick(CInputManager& _manager, float deltaTime) override;

};

#endif