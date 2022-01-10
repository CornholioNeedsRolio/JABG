#include "CCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "CFrustumCollider.h"
#include <iostream>
#include "CMesh.h"
#include <glm/glm.hpp>
#include <iostream>


CCamera::CCamera(SVector3 _positon, SVector3 _rotation) : 
	CObject(_positon, _rotation), m_collider(this)
{
	flags = 0;
	is_ortho = 0;
}

void CCamera::MakePerspective(float fovy, float aspect, float near, float far)
{
	flags &= ~IS_ORTHO;
	is_ortho = 0;
	m_projection = glm::perspective(glm::radians(fovy), aspect, near, far);
}

void CCamera::MakeOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	flags |= IS_ORTHO;
	is_ortho = 1;
	m_projection = glm::ortho(left, right, bottom, top, zNear, zFar);
}

SMat4 CCamera::getProjection() const
{
	return m_projection;
}

SMat4 CCamera::getView() const
{
	glm::mat4 output = glm::mat4(1);
	output *= glm::rotate(glm::mat4(1), glm::radians(-GetGlobalRotation().x), { 1, 0, 0 }); //pitch
	output *= glm::rotate(glm::mat4(1), glm::radians(-GetGlobalRotation().y), { 0, 1, 0 }); //yaw
	output *= glm::rotate(glm::mat4(1), glm::radians(-GetGlobalRotation().z), { 0, 0, 1 }); //roll

	output *= glm::translate(glm::mat4(1), -GetGlobalPosition());

	return output;
}

CFrustumCollider& CCamera::getCollider()
{
	return m_collider;
}

void CCamera::makeMesh()
{

	glm::mat4 invproj = glm::inverse(m_projection);
	std::vector<glm::vec4> margins = 
	{
		{ 1, 1, 1, 1},
		{-1,-1,-1, 1},
	};
	for(auto& v : margins)
	{
		if(isOrtho())
			v = v * invproj;
		else 
			v *= v.w;
	}

	if(!m_mesh)
		m_mesh = new CMesh;
	else
		m_mesh->Clear();

	m_mesh->InitDebugRect(margins[0].x, margins[0].y, margins[0].z, margins[1].x, margins[1].y, margins[1].z);
	m_mesh->Attach(this);
}

bool CCamera::isOrtho() const
{
	return is_ortho;

	if(flags & IS_ORTHO)
		return true;
	return false;
}

glm::vec3 CCamera::getMiddle() const
{
	glm::mat4 invproj = glm::inverse(m_projection);
	std::vector<glm::vec4> margins = 
	{
		{ 1, 1, 1, 1},
		{ 1,-1, 1, 1},
		{-1,-1, 1, 1},
		{-1, 1, 1, 1},
		{ 1, 1,-1, 1},
		{ 1,-1,-1, 1},
		{-1,-1,-1, 1},
		{-1, 1,-1, 1}
	};

	float minx = INFINITY, miny = INFINITY, minz = INFINITY, maxx= -INFINITY, maxy = -INFINITY, maxz = -INFINITY;
	for(auto& v : margins)
	{
		v = invproj * v;
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;
		minx = std::min(minx, v.x);
		miny = std::min(miny, v.y);
		minz = std::min(minz, v.z);

		maxx = std::max(maxx, v.x);
		maxy = std::max(maxy, v.y);
		maxz = std::max(maxz, v.z);
	}
	//for(int i = 0; i < 8; ++i)
	//	printf("%f %f %f %f\n", margins[i].x, margins[i].y, margins[i].z, margins[i].w);

	//printf("%f %f %f %f %f %f \n", minx, miny, minz, maxx, maxy, maxz);
	glm::vec4 output(minx+maxx, miny+maxy, minz+maxz, 1.0);
	output = output * 0.5f * getView();
	return output;
}

void CCamera::Draw(const SDrawInfo& info)
{
	if(m_mesh)
	{
		m_mesh->Draw(info);
	}
}

CCamera::~CCamera()
{
	if(m_mesh)
		delete m_mesh;
}
