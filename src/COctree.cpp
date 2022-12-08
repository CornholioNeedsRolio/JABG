#include "COctree.h"
#include <iostream>

COctree::COctree(const glm::vec3& position, const glm::vec3& sizes, const glm::vec3& minsize, bool expandable) :
	m_boundbox(position, position+sizes)
{
	m_position = position;
	m_sizes = sizes;
	m_divided = false;
	m_children.fill(nullptr);
	m_info = 0;
	m_minsize = minsize;
	m_expandable = expandable;
}

float COctree::getInfo(const glm::vec3& position)
{
	if (m_divided)
	{
		for(int i = 0; i < 8; i++)
			if (m_children[i]->containsPoint(position))
			{
				return m_children[i]->getInfo(position);
			}
	}
	return m_info;
}

void COctree::setInfo(const glm::vec3& position, const float& info)
{
	if (containsPoint(position))
	{
		Divide();
		if (!m_divided)
			m_info = info;
		else
		{
			for (int i = 0; i < 8; i++)
			{
				if (m_children[i]->containsPoint(position))
				{
					m_children[i]->setInfo(position, info);

					bool shouldMerge = true;
					for (int j = 0; j < 8; j++)
						shouldMerge &= m_children[j]->shouldMerge(info);

					if (shouldMerge)
					{
						m_info = info;
						for (int j = 0; j < 8; j++)
							m_children[j] = nullptr;
						m_divided = false;
					}
					return;
				}
			}
		}
	}
	else
	{
		Expand(position);
		//setInfo(position, info);
	}
}

void COctree::Divide()
{
	if (m_divided)
		return;

	glm::vec3 halfs = m_sizes * 0.5f;
	if (halfs.x < m_minsize.x && halfs.y < m_minsize.y && halfs.z < m_minsize.z)
		return;


	m_children[OCT_NW] = std::make_shared<COctree>(m_position												, halfs, m_minsize);
	m_children[OCT_NE] = std::make_shared<COctree>(m_position + glm::vec3(halfs.x	, 0			, 0)		, halfs, m_minsize);
	m_children[OCT_SW] = std::make_shared<COctree>(m_position + glm::vec3(0			, 0			, halfs.z)	, halfs, m_minsize);
	m_children[OCT_SE] = std::make_shared<COctree>(m_position + glm::vec3(halfs.x	, 0			, halfs.z)	, halfs, m_minsize);
	
	m_children[OCT_NWU] = std::make_shared<COctree>(m_position + glm::vec3(0		, halfs.y	, 0)		, halfs, m_minsize);
	m_children[OCT_NEU] = std::make_shared<COctree>(m_position + glm::vec3(halfs.x	, halfs.y	, 0)		, halfs, m_minsize);
	m_children[OCT_SWU] = std::make_shared<COctree>(m_position + glm::vec3(0		, halfs.y	, halfs.z)	, halfs, m_minsize);
	m_children[OCT_SEU] = std::make_shared<COctree>(m_position + glm::vec3(halfs.x	, halfs.y	, halfs.z)	, halfs, m_minsize);

	m_divided = true;
}

void COctree::Expand(const glm::vec3& point)
{
	if (m_expandable)
	{
		glm::vec3 position = (glm::ceil(glm::abs(point / m_sizes))) * m_sizes *glm::sign(point);

		if (point.x <= m_sizes.x && point.x >= m_position.x)
			position.x = m_position.x;		   
		if (point.y <= m_sizes.y && point.y >= m_position.y)
			position.y = m_position.y;		   
		if (point.z <= m_sizes.z && point.z >= m_position.z)
			position.z = m_position.z;

		std::cout << position.x << ' ' << position.y << ' ' << position.z << '\n';
	}
}

void COctree::Draw(const SDrawInfo& info)
{
	if (m_divided)
	{
		for (int i = 0; i < 8; i++)
			m_children[i]->Draw(info);
	}
	else
		m_boundbox.Draw(info);
}

bool COctree::containsPoint(const glm::vec3& point)
{
	return m_boundbox.collidesWithPoint(point);
}

bool COctree::shouldMerge(float info)
{
	return info == m_info && !m_divided;
}

void COctree::setChild(char child, const std::shared_ptr<COctree>& tree)
{
	Divide();
	m_children[child] = tree;
}
