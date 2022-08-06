#ifndef CHUNK_HEADER
#define CHUNK_HEADER
#include "ChunkComponents/CChunkVoxelComponent.h"
#include "ChunkComponents/CChunkMeshComponent.h"
#include "ChunkComponents/CChunkSaveComponent.h"
#include "Engine/CTextureAtlas.h"
#include "Engine/CObject.h"
#include "Engine/CMesh.h"
#include "CBlock.h"
#include <atomic>
#include <memory>
#include <map>


enum
{
	CHUNKFACE_TOP,
	CHUNKFACE_BOT,
	CHUNKFACE_LEFT,
	CHUNKFACE_RIGHT,
	CHUNKFACE_FRONT,
	CHUNKFACE_BACK
};

class CChunk : public CObject
{
	CChunkVoxelComponent m_voxelComponent;
	CChunkMeshComponent m_meshComponent;
	CChunkSaveComponent m_saveComponent;

	glm::ivec3 m_position;
	class CChunkManager* m_manager = 0x0;

public:
	CChunk(glm::ivec3 position, class CChunkManager* manager = 0x0);
	virtual ~CChunk() override;

	CChunkVoxelComponent& getVoxelComponent();
	CChunkMeshComponent& getMeshComponent();
	CChunkSaveComponent getSaveComponent();

	void setManager(class CChunkManager* manager = 0x0);
	CChunkManager* getManager() const;

	const glm::ivec3& getChunkPosition() const;

	void BulkDraw(class CBulkRenderer* renderer) override;
	bool fustumCulling(class CCamera* camera) const override;

	void makeNeighborDirty(uint8_t card);
	void makeNeighborsDirty();
};

#endif
