#ifndef CHUNKPART_HEADER
#define CHUNKPART_HEADER
#include "Engine/CObject.h"
#include "CChunk.h"

#include <map>
#include <unordered_map>
#include <thread>

class CChunkPart : public CObject
{
	class CWorld* m_world;
	std::unordered_map<int, std::shared_ptr<CChunk>> m_chunks;
	glm::ivec2 m_position;
public:
	CChunkPart(class CWorld* world, glm::ivec2 position);

	CChunk* getChunk(int y);    /*if chunk doesn't exist it return nullptr*/
	CChunk* getChunkNew(int y); /*if chunk doesn't exist it will create a new one*/
	CChunk* addChunk(const std::shared_ptr<CChunk>& chunk); /*adds chunk to the chunk_part*/

	CChunk* getChunkInRange(int closeToY, bool fuckingClean = false, int *pos = nullptr);

	void MakeDirty();
	bool destroyable();
	void UnloadChunks(int cy, int halfRenderDistance);
	void Draw(const SDrawInfo& info) override;
	void BulkDraw(class CBulkRenderer* renderer) override;

	std::unordered_map<int, std::shared_ptr<CChunk>>::iterator getBeginIterator();
	std::unordered_map<int, std::shared_ptr<CChunk>>::iterator getEndIterator();

	glm::ivec2& getChunkPartPosition();
};

#endif