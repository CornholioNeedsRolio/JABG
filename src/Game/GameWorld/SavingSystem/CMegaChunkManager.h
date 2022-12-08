#ifndef JABG_CMEGACHUNKMANAGER_H
#define JABG_CMEGACHUNKMANAGER_H
#include "CMegaChunk.h"
#include <vector>
#include <memory>

class CMegaChunkManager
{
	std::vector<std::shared_ptr<CMegaChunk>> m_megaChunks;
	class CWorld* m_world;
	std::mutex m_mutex;
	float m_saveTimer = 0;
public:
	CMegaChunkManager(CWorld* world);
	std::shared_ptr<CMegaChunk> GetMegaChunk(int x, int y, int z);
	void ForceSave();
	void Tick(float DeltaTime);
};


#endif //JABG_CMEGACHUNKMANAGER_H
