#ifndef CCHUNKMANAGER_HEADER
#define CCHUNKMANAGER_HEADER
#include <map>
#include "Engine/CObject.h"
#include "Game/CChunkPart.h"
#include "Game/ChunkManager/SavingChunks/CSaveChunkManager.h"

using UChunkPos = std::tuple<int, int, int>;
using UChunkPartPos = std::pair<int, int>	;
using ULocalPos = std::tuple<char, char, char>;

class CChunkManager : public CObject
{
	class CWorld* m_world = nullptr;
	std::map<UChunkPartPos, CChunkPart> m_chunks;
	std::vector<std::shared_ptr<CSaveChunkManager>> m_loadedSaves;
	std::mutex m_mutex;
public:
	CChunkManager(class CWorld* world);
	CChunkPart* getChunkPart(int x, int z);

	CWorld* getWorld();

	std::shared_ptr<CSaveChunkManager> GetLoadedMegaChunk(int x, int y, int z);

	CChunkPart* createChunkIfNone(int x, int z, bool* existed = 0x0);
	CChunkPart* createChunkIfNoneGenerated(int x, int z);

	std::map<UChunkPartPos, CChunkPart>::iterator getBeginIterator();
	std::map<UChunkPartPos, CChunkPart>::iterator getEndIterator();

	void Draw(const SDrawInfo& info) override;
	void Tick(CInputManager& _manager, float deltaTime) override;
	void BulkDraw(class CBulkRenderer* renderer) override;
	void Clear(const std::tuple<int,int,int>& playerPos);

	static std::pair<UChunkPos, ULocalPos>	getLocal(int x, int y, int z);
};

#endif