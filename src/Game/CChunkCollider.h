#ifndef CChunkCollider_Header
#define CChunkCollider_Header

#include <array>
#include "../CAABB.h"
#include "Engine/Objects/CObject.h"
#include "Game/Chunk/CChunk.h"
#include "GameImportantStuff.h"

/*
	*This class is meant to make collision more efficient
	*At it's core it's just a octree
	*I think this is the strangest implementation ever
*/

class CChunkCollider : public CObject
{
	std::unique_ptr<CChunkCollider[]> m_octree = nullptr;
	ColliderType m_info = BLOCKCOLLIDER_NONE;
	glm::vec3 m_pos, m_size;
	CAABB m_collider;

	void Divide(); /*might crash, not good practice but I don't want to check twice if m_octree is not null, "don't say I didn't warned ya, kiddo"*/

	enum
	{
		OCT_sw,
		OCT_se,
		OCT_wn,
		OCT_ne,
		OCT_swu,
		OCT_seu,
		OCT_wnu,
		OCT_neu,
	};
public:
	CChunkCollider(glm::vec3 position, glm::vec3 size);
	~CChunkCollider();
	
	void Draw(const SDrawInfo& info) override;
	void setInfo(int x, int y, int z, ColliderType type);
	void setInfo(ColliderType x);
	bool shouldMerge(ColliderType type);
	bool intersectsPoint(float x, float y, float z);

	bool applyCollisionWithGEC(CAABB& other, glm::vec3& velocity, std::vector<std::pair<CAABB*, float>>& collisions);
	bool RayCastWithGEC(const glm::vec3& position, const glm::vec3& offset, std::vector<std::pair<glm::vec3, float>>& collisions);

	const ColliderType& getType(bool& divided);

	bool Fractured();
	CAABB& getCollider();
};

#endif