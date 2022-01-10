#ifndef CHUNK_HEADER
#define CHUNK_HEADER
#include "../CObject.h"
#include "../CMesh.h"
#include "../CTextureAtlas.h"
#include "CBlock.h"
#include <atomic>
#include <memory>
#include <map>

#define CHUNK_SIZE 16
using blocktype = unsigned short;
using blockarray = std::array<blocktype, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>;


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
	std::unique_ptr<blocktype[]> m_blocks = 0x0; //using too many pointers, I know but I want to avoid memory consumtion 

	std::vector<SVertex> m_meshinfo;
	glm::ivec3 m_position;
	CMesh m_mesh;
	static std::unique_ptr<CMesh> m_bounds;
	class CChunkManager* m_manager = 0x0;

	class CChunkCollider* m_collider = 0x0;

	std::map<std::tuple<int, int, int>, int> m_edits;
	std::atomic_char m_flags;
	uint8_t m_nflags; //normal flags
	enum
	{
		THREAD_RUNNING = 0x1,
		KILL_THREAD = 0x2,
		THREAD_COMPLETED = 0x4,
		CHUNK_DIRTY = 0x8,
		IMPORTANT_NEIGHBOR = 0x10,
		GENERATING_CHUNK = 0x20,
		CONSUME_EDITS = 0X40
	};

	enum
	{
		NORMAL_DEBUG = 0x1,
		GENERATE_COLLIDER = 0x2
	};

	CBlock* getBlockPtr(int x, int y, int z);
public:
	CChunk(glm::ivec3 position, class CChunkManager* manager = 0x0);
	CChunk(CChunk&& A);
	~CChunk();

	void setManager(class CChunkManager* manager = 0x0);
	void setTextureAndShader(std::shared_ptr<CTextureAtlas> texture, std::shared_ptr<CShader> shader);
	blocktype GetBlock(int x, int y, int z);
	uint8_t SetBlock(int x, int y, int z, blocktype id);
	void Draw(const SDrawInfo& info) override;
	void BulkDraw(class CBulkRenderer* renderer) override;
	void Tick(CInputManager& _manager, float deltaTime) override;
	bool fustumCulling(class CCamera* camera) const override;

	void setAllBlocks(std::unique_ptr<blocktype[]> blocks);

	void BuildMesh(std::array<CChunk*, 6> neighbors, std::shared_ptr<CTextureAtlas> atlas);

	const glm::ivec3& getChunkPosition() const;
	bool isChunkDirty();
	void MakeDirty();
	void makeNeighborDirty(uint8_t card);
	void makeNeighborsDirty();
	bool consumeEdits();

	bool isChunkGenerating();
	void setChunkGenerating(bool value);

	bool canDestroy();
	void setDestroyable(bool destroyable);

	bool intersectsCollider(class CAABB* other, const glm::vec3& velocity);
	void generateCollider(bool force = false);
	void emptyCollider();

	void MakeCollider();
	void setBlockInCollider(int x, int y, int z, blocktype id);

	class CChunkCollider* getCollider();

	CChunk& operator=(const CChunk& A);
};

#endif
