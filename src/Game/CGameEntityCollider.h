#ifndef CGameEntityCollider_Header
#define CGameEntityCollider_Header
#include "../CAABB.h"


enum
{
	EntityC_Up		 = 0x1,
	EntityC_Down	 = 0x2,
	EntityC_Left	 = 0x4,
	EntityC_Right	 = 0x8,
	EntityC_Front	 = 0x10,
	EntityC_Back	 = 0x20,
};

class CGameEntityCollider
{
	CAABB m_collider;
public:
	CGameEntityCollider(const glm::vec3& min, const glm::vec3& max);

	uint8_t Move(glm::vec3& velocity, class CChunkManager* manager);
	bool RayCast(const glm::vec3& position, const glm::vec3& offset, glm::vec3& worldpos, glm::vec3& normal, class CChunkManager* manager);

	void forceRegen(glm::ivec3 position, class CChunkManager* manager);
	CAABB& getCollider();
};

#endif