#include "CAABB.h"
#include "glm/gtx/string_cast.hpp"

#include <glm/gtx/component_wise.hpp>
#include "CFileManager.h"
#include <algorithm>
#include <iostream>


#ifdef DEBUG_DRAW_COLLIDER
CMesh CAABB::m_mesh;
#endif

CAABB::CAABB(const glm::vec3& min, const glm::vec3& max)
{
	m_pivot = (min + max)/2.f;
	m_size = (max - min) / 2.f;

	//std::cout << glm::to_string(m_size) + ' ' + glm::to_string(m_pivot) + '\n';
}

glm::vec3 CAABB::getMinVector(bool local)
{
	if(!local)
		return GetGlobalPosition() + m_pivot - m_size;
	return m_pivot - m_size;
}

glm::vec3 CAABB::getMaxVector(bool local)
{
	if (!local)
		return GetGlobalPosition() + m_pivot + m_size;
	return m_pivot + m_size;
}

glm::vec3 CAABB::getCenterVector(bool local)
{
	if (!local)
		return GetGlobalPosition() + m_pivot;
	return m_pivot;
}

glm::vec3 CAABB::getSizeVector()
{
	return m_size;
}

void CAABB::setDebugColor(bool color)
{
#ifdef DEBUG_DRAW_COLLIDER
	m_mesh.setColor(color ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 0));
#endif
}

bool CAABB::applyCollision(CAABB& aabb, float push, bool move, glm::vec3* velocity)
{
	glm::vec3 otherPosition = aabb.getCenterVector();
	//if (velocity)
	//	otherPosition += *velocity;
	glm::vec3 otherSize = aabb.getSizeVector();
	glm::vec3 selfPosition = getCenterVector();
	glm::vec3 selfSize = getSizeVector();

	glm::vec3 delta = otherPosition - selfPosition;

	glm::vec3 intersection = glm::abs(delta) - (otherSize + selfSize);

	if (intersection.x < 0 && intersection.y < 0 && intersection.z < 0)
	{
		if (move)
		{
			push = push > 1 ? 1 : (push < 0 ? 0 : push);
			if (intersection.y > intersection.x && intersection.y > intersection.z)
			{
				if (delta.y > 0.f)
				{
					Move({ 0, intersection.y * (1 - push), 0 });
					aabb.Move({ 0, -intersection.y * push, 0 });
				}
				else
				{
					Move({ 0, -intersection.y * (1 - push), 0 });
					aabb.Move({ 0, intersection.y * push, 0 });
				}
				if (velocity)
					velocity->y = 0;
			}
			else if (intersection.z > intersection.x)
			{
				if (delta.z > 0.f)
				{
					Move({ 0, 0, intersection.z * (1 - push) });
					aabb.Move({ 0, 0, -intersection.z * push });
				}
				else
				{
					Move({ 0, 0, -intersection.z * (1 - push) });
					aabb.Move({ 0, 0, intersection.z * push });
				}
				if (velocity)
					velocity->z = 0;
			}
			else
			{
				if (delta.x > 0.f)
				{
					Move({ intersection.x * (1 - push), 0, 0 });
					aabb.Move({ -intersection.x * push, 0, 0 });
				}
				else
				{
					Move({ -intersection.x * (1 - push), 0, 0 });
					aabb.Move({ intersection.x * push, 0, 0 });
				}
				if (velocity)
					velocity->x = 0;
			}
		}
		return true;
	}


	return false;
}


bool CAABB::sweepCollision(CAABB& aabb, glm::vec3& velocity, bool apply)
{
	if (std::abs(velocity.x) < m_size.x &&
		std::abs(velocity.y) < m_size.y &&
		std::abs(velocity.z) < m_size.z)
	{
		if(apply)
			Move(velocity);
		bool result = applyCollision(aabb, 0, apply);
		return result;
	}


	glm::vec3 epsilon = glm::vec3(0.01)*glm::sign(velocity);
	Move(epsilon);
	if (applyCollision(aabb, 0, true, &velocity))
	{
		Move(-epsilon);
		if(apply)
			Move(velocity);
		return true;
	}
	Move(-epsilon);

	float delta = 0;
	//if (lineIntersection(aabb, getCenterVector(), velocity, &delta, getSizeVector()))
	{
		if (apply)
			Move(velocity * delta);
		return true;
	}

	if (apply)
		Move(velocity);
	
	return false;
}


/*
	SECOND BIGGEST SPAGHETTY MONSTER I'VE CREATED(first is the text), I MADE IT WORK, LIKE A GENETIC ALGORITHM, RANDOM CHANGES UNTIL IT WORKS.
	if someone else than me sees this, I hope you don't get mad, after all 2020 was not a great year
*/
bool CAABB::RayCast(const glm::vec3& position, const glm::vec3& offset, glm::vec3& normal, float& delta, bool* inside)
{
glm::vec3 NearUndiv, FarUndiv, aabbMin = getMinVector() , aabbMax =  getMaxVector() ;
	NearUndiv = aabbMax - position;
	FarUndiv = aabbMin - position;
	if (inside)
		*inside = FarUndiv.x <= 0 && FarUndiv.y <= 0 && FarUndiv.z <= 0 && NearUndiv.x >= 0 && NearUndiv.y >= 0 && NearUndiv.z >= 0;
	glm::vec3 invDir = 1.f / offset;
	glm::vec3 Near = NearUndiv * invDir;
	glm::vec3 Far = FarUndiv * invDir;
	normal = { 0,0,0 };
	
	if (Near.x > Far.x) std::swap(Near.x, Far.x);
	if (Near.y > Far.y) std::swap(Near.y, Far.y);
	if (Near.z > Far.z) std::swap(Near.z, Far.z);

	if(Near.x >= Far.y || Near.x >= Far.z) return false;
	if(Near.y >= Far.z || Near.y >= Far.x) return false;
	if(Near.z >= Far.y || Near.z >= Far.x) return false;

	delta = std::max({ Near.x, Near.y, Near.z });
	float hitFar = std::min({ Far.x, Far.y, Far.z });
	if (hitFar <= 0) return false;
	if (delta < 0) return false;

	const glm::vec3 hitPoint = position + offset * delta;

	int normals = 0;
	for(int i = 0; i < 3; ++i)
	{
		if(glm::abs(hitPoint[i] - aabbMin[i] ) == 0)
		{
			normal[i] = -1;
			++normals;
			continue;
		}
		if(glm::abs(hitPoint[i] - aabbMax[i]) == 0)
		{
			normal[i] = 1;
			++normals;
		}
	}

	if(normals > 1) normal = glm::vec3(0);

	/*int index = 0;
	for (int i = 1; i < 3; ++i)
		if (Near[i] >= Near[index])
			index = i;
	normal = glm::vec3(0);
	
	if(glm::abs(invDir[index]) > 0.1)
		normal[index] = -glm::sign(invDir[index]);

	int patience = 0;
	for(int i = 0; i < 3; ++i)
	{
		if(NearUndiv[i] == 0)
			if(patience++ >= 1)
				normal = glm::vec3(0);
	}*/
	return true;
}

bool CAABB::RaySweep(CAABB& box, const glm::vec3& offset, glm::vec3& normal, float& delta, bool* inside)
{
	struct sizer {
		CAABB *aabb, *t;
		glm::vec3 originalSize;
		sizer(CAABB* aabb, CAABB* t) : aabb(aabb), t(t) { originalSize = aabb->m_size; for(int i = 0; i < 3; ++i) aabb->m_size[i] += t->m_size[i]; };
		~sizer() { aabb->m_size = originalSize; };
	} s(&box, this);	

	if (box.RayCast(getCenterVector(), offset, normal, delta, inside))
	//if(lineIntersection(box, getCenterVector(), offset, normal, &delta))
	{
		if (delta <= 1)
			return true;
	}
	return false;
}


bool CAABB::ResolveDynamicSweep(CAABB& box, glm::vec3& velocity, glm::vec3& normal, bool forceNormalUp)
{
	float delta;
	if (RaySweep(box, velocity, normal, delta))
	{
		if(forceNormalUp)
			normal.y = 1;
		velocity += normal * glm::abs(velocity) * (1.f - delta);
		//std::cout << "helo";
		//std::cout << glm::to_string(normal) + '\n';
		return true;
	}
	return false;
}


bool CAABB::collidesWithPoint(glm::vec3 point)
{
	glm::vec3 min = getMinVector();
	glm::vec3 max = getMaxVector();
	bool x = min.x <= point.x && max.x >= point.x;
	bool y = min.y <= point.y && max.y >= point.y;
	bool z = min.z <= point.z && max.z >= point.z;

	return x && y && z;
}


uint8_t CAABB::checkIntersetion(CAABB& other)
{
	glm::vec3 otherPosition = other.getCenterVector();
	glm::vec3 otherSize = other.getSizeVector();
	glm::vec3 selfPosition = getCenterVector();
	glm::vec3 selfSize = getSizeVector();

	glm::vec3 delta = otherPosition - selfPosition;
	glm::vec3 intersection = glm::abs(delta) - (otherSize + selfSize);
	
	if (intersection.x >= 0 || intersection.y >= 0 || intersection.z >= 0) return AABB_NotIntersected;

	if (intersection.x <= -2*m_size.x && intersection.y <= -2 * m_size.y && intersection.z <= -2 * m_size.z) return AABB_FullyIntersected;
	if (intersection.x < 0 && intersection.y < 0 && intersection.z < 0) return AABB_PartiallyIntesected;

	return 0;
}		

void CAABB::Draw(const SDrawInfo& info)
{
	#ifdef DEBUG_DRAW_COLLIDER

	if (info.mode == NORMAL_DRAW)
	{
		if (!m_mesh.isInit())
		{
			m_mesh.InitDebugRect(0, 0, 0, 1, 1, 1);

			//m_mesh.SetShader(CFileManager::getShader("./res/DefaultShader/DefaultShader"));
		}
		m_mesh.setColor(glm::vec3(0));
		m_mesh.SetPosition(getCenterVector()-m_size);
		m_mesh.setSize(m_size*2.f);

		//glm::vec3 pos = GetGlobalPosition();
		//m_mesh.SetGlobalPosition(pos);
		m_mesh.Draw(info);
	}

	#endif
}

void CAABB::BulkDraw(class CBulkRenderer* renderer)
{
	if (!m_mesh.isInit())
	{
		m_mesh.InitDebugRect(0, 0, 0, 1, 1, 1);
	}
	m_mesh.setColor(glm::vec3(0));
	m_mesh.SetPosition(getCenterVector()-m_size);
	m_mesh.setSize(m_size*2.f);

	m_mesh.BulkDraw(renderer);
}

#define outvec(vec) std::cout << vec.x << ' ' << vec.y << ' ' << vec.z << '\n';
bool lineQuadIntersection(SQuad quad, glm::vec3 pos, glm::vec3 offset, float& delta)
{
	//3 points to calculate the plane
	glm::vec3 A0 = quad.s_pos0;
	glm::vec3 A1 = A0;

	bool onZ = false, onX = false;

	if (A1.z != quad.s_pos1.z)
	{
		A1.z = quad.s_pos1.z;
		onX = true;
	}
	else
		A1.y = quad.s_pos1.y;

	glm::vec3 A2 = A0;
	if (A1.x != quad.s_pos1.x)
	{
		A2.x = quad.s_pos1.x;
		onZ = true;
	}
	else
		A2.y = quad.s_pos1.y;

	A1 = A1 - A0;
	A2 = A2 - A0;
	/*
	3d Cross product to get plane normal
	*/
	glm::vec3 planeindices;
	planeindices.x = A1.y * A2.z - A2.y * A1.z;
	planeindices.y = A1.z * A2.x - A1.x * A2.z;
	planeindices.z = A1.x * A2.y - A2.x * A1.y;

	//d = ax+by+cz
	float dist = planeindices.x * A0.x + planeindices.y * A0.y + planeindices.z * A0.z;
	//std::cout << planeindices.x << ' ' << planeindices.y << ' ' << planeindices.z << ' ' << dist << '\n';
	//A = pos + delta*offset
	delta = (dist - planeindices.x * pos.x - planeindices.y * pos.y - planeindices.z * pos.z) / (planeindices.x * offset.x + planeindices.y * offset.y + planeindices.z * offset.z);
	//delta = std::abs(delta);
	//checking if delta is within segment
	if (delta >= 0 && delta <= 1)
	{

		glm::vec3 point = pos + delta * offset;
		//checking if it is within quad

		if (onX)
			if (quad.s_pos0.y < point.y && quad.s_pos0.z < point.z && 
				quad.s_pos1.y > point.y && quad.s_pos1.z > point.z)
				return true;
		if (onZ)
			if (quad.s_pos0.x < point.x && quad.s_pos0.y < point.y &&
				quad.s_pos1.x > point.x && quad.s_pos1.y > point.y)
				return true;

		if (quad.s_pos0.x < point.x && quad.s_pos0.z < point.z &&
			quad.s_pos1.x > point.x && quad.s_pos1.z > point.z)
			return true;

	}
	delta = 1;
	return false;
}

bool lineIntersection(CAABB& aabb, glm::vec3 pos, glm::vec3 offset, glm::vec3& normal, float* delta, glm::vec3 sizes)
{
	/*increasing target's volume with sizes from first box, 0 if normal raycast*/
	glm::vec3 othermin = aabb.getMinVector() - sizes;
	glm::vec3 othermax = aabb.getMaxVector() + sizes;


	//Getting box points, so I can pass faces
	glm::vec3 x0y0z0 = othermin;

	glm::vec3 x1y0z0 = x0y0z0;
	x1y0z0.x = othermax.x;

	glm::vec3 x0y0z1 = x0y0z0;
	x0y0z1.z = othermax.z;

	glm::vec3 x1y0z1 = x0y0z1;
	x1y0z1.x = othermax.x;


	glm::vec3 x0y1z0 = x0y0z0;
	x0y1z0.y = othermax.y;

	glm::vec3 x1y1z0 = x1y0z0;
	x1y1z0.y = othermax.y;

	glm::vec3 x0y1z1 = x0y0z1;
	x0y1z1.y = othermax.y;

	glm::vec3 x1y1z1 = x1y0z1;
	x1y1z1.y = othermax.y;

	//Checking each other
	float min = INFINITY;
	float tempdelta = 0;

	/*SQuad faces[6] =
	{
		{ x0y0z0, x1y1z0 },
		{ x0y0z1, x1y1z1 },
		{ x1y0z0, x1y1z1 },
		{ x0y0z0, x0y1z1 },
		{ x0y1z0, x1y1z1 },
		{ x0y0z0, x1y0z1 }
	};*/

	std::pair<SQuad, glm::vec3> facesn[6] =
	{
		{ { x0y0z0, x1y1z0 },{0,0,-1}},
		{ { x0y0z1, x1y1z1 },{0,0,1}},
		{ { x1y0z0, x1y1z1 },{1,0,0}},
		{ { x0y0z0, x0y1z1 },{-1,0,0}},
		{ { x0y1z0, x1y1z1 },{0,1,0}},
		{ { x0y0z0, x1y0z1 },{0,-1,0}},
	};

	for(int i = 0; i < 6; i++)
		if (lineQuadIntersection(facesn[i].first, pos, offset, tempdelta))
		{
			normal = facesn[i].second;
			min = std::min(tempdelta, min);
		}

	if (min != INFINITY)
	{
		if(delta)
			*delta = min;
		return *delta >= 0;
	}
	if(delta)
		*delta = 1;
	return false;
}
