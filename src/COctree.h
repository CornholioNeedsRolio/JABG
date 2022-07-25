#ifndef COCTREE_HEADER
#define COCTREE_HEADER
#include "Engine/CObject.h"
#include "CAABB.h"
#include <array>
#include <memory>
#include <vector>

enum
{
	OCT_NW,
	OCT_NE,
	OCT_SW,
	OCT_SE,
	OCT_NWU,
	OCT_NEU,
	OCT_SWU,
	OCT_SEU,
};

class COctree : public CObject
{
	glm::vec3 m_position;
	glm::vec3 m_sizes;
	glm::vec3 m_minsize;
	float m_info;
	CAABB m_boundbox;

	uint8_t m_divided;
	uint8_t m_expandable;

	std::array<std::shared_ptr<COctree>, 8> m_children;
public:
	COctree(const glm::vec3& position, const glm::vec3& sizes, const glm::vec3& minsize, bool expandable = false);
	float getInfo(const glm::vec3& position);
	void setInfo(const glm::vec3& position, const float& info);
	void Divide();
	void Expand(const glm::vec3& point);

	void Draw(const SDrawInfo& info);
	bool containsPoint(const glm::vec3& point);
	bool shouldMerge(float info);
	void setChild(char child, const std::shared_ptr<COctree>& tree);
};

#endif