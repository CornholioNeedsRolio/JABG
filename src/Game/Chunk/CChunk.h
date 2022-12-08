#ifndef CHUNK_HEADER
#define CHUNK_HEADER
#include "Game/Chunk/ChunkComponents/CChunkVoxelComponent.h"
#include "Game/Chunk/ChunkComponents/CChunkMeshComponent.h"
#include "Game/Chunk/ChunkComponents/CChunkSaveComponent.h"
#include "Game/Chunk/ChunkComponents/CChunkLightComponent.h"
#include "Engine/Objects/CObject.h"
#include "Engine/GLRenderer/CMesh.h"
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
	CChunkLightComponent m_lightComponent;

	glm::ivec3 m_position;
	class CChunkManager*	m_manager	= nullptr;
	class CChunkPart*		m_parent	= nullptr;
	std::vector<IChunkComponent*> m_components;
public:
	CChunk(glm::ivec3 position, class CChunkManager* manager = 0x0);
	virtual ~CChunk() override;

	CChunkVoxelComponent& getVoxelComponent();
	CChunkMeshComponent& getMeshComponent();
	CChunkSaveComponent& getSaveComponent();
	CChunkLightComponent& getLightComponent();

	void setManager(class CChunkManager* manager, class CChunkPart* part);
	CChunkManager* getManager() const;
	CChunkPart* getChunkPart() const;

	void MakeDirty();

	const glm::ivec3& getChunkPosition() const;

	void BulkDraw(class CBulkRenderer* renderer) override;
	bool fustumCulling(class CCamera* camera) const override;

	void makeNeighborDirty(uint8_t card);
	void makeNeighborsDirty();
};

#endif
