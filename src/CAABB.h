#ifndef CAABB_HEADER
#define CAABB_HEADER
#include <glm/glm.hpp>
#include "Engine/CObject.h"
#include "Engine/CMesh.h"

#define DEBUG_DRAW_COLLIDER

enum
{
	AABB_NotIntersected = 0,
	AABB_FullyIntersected = 1,
	AABB_PartiallyIntesected = 2,
};

enum
{
	AABB_Normal_Up,		 //(0,1,0)
	AABB_Normal_Down,	 //(0,-1,0)

	AABB_Normal_Left,	 //(-1,0,0)
	AABB_Normal_Right,	 //(1,0,0)

	AABB_Normal_Front,   //(0,0,1)
	AABB_Normal_Back,    //(0,0,-1)
};

class CAABB : public CObject
{
	glm::vec3 m_pivot;
	glm::vec3 m_size;

	#ifdef DEBUG_DRAW_COLLIDER
	static CMesh m_mesh;
	#endif
public:
	CAABB(const glm::vec3& min, const glm::vec3& max);

	glm::vec3 getMinVector(bool local = false);
	glm::vec3 getMaxVector(bool local = false);
	glm::vec3 getCenterVector(bool local = false);
	glm::vec3 getSizeVector();

	void setDebugColor(bool color);
	bool applyCollision(CAABB& aabb, float push, bool move = true, glm::vec3* velocity = nullptr);
	bool sweepCollision(CAABB& aabb, glm::vec3& velocity, bool apply = true);


	bool RayCast(const glm::vec3& position, const glm::vec3& offset, glm::vec3& normal, float& delta, bool *inside = nullptr);
	bool RaySweep(CAABB& box, const glm::vec3& offset, glm::vec3& normal, float& delta, bool* inside = nullptr);
	bool ResolveDynamicSweep(CAABB& box, glm::vec3& velocity, glm::vec3& normal, bool forceNormalUp = false);

	bool collidesWithPoint(glm::vec3 point);

	uint8_t checkIntersetion(CAABB& other);

	void Draw(const SDrawInfo& info) override;
	void BulkDraw(class CBulkRenderer* renderer) override;
};

struct SQuad
{
	glm::vec3 s_pos0;
	glm::vec3 s_pos1;
};

bool lineQuadIntersection(SQuad quad, glm::vec3 pos, glm::vec3 offset, float& delta);
bool lineIntersection(CAABB& aabb, glm::vec3 pos, glm::vec3 offset, glm::vec3& normal, float* delta, glm::vec3 sizes = glm::vec3(0));

#endif