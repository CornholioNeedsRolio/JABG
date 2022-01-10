#include "CFPPlayer.h"
#include "../Game/CWorld.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "../CFileManager.h"

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	assert(!(hi < lo));
	return (v < lo) ? lo : (hi < v) ? hi : v;
}


float CFPPlayer::jumpHyperbola(float value)
{
	float t = 0.5f - value;
	return -t*t * 4;
}

CFPPlayer::CFPPlayer(SVector3 _pos, SVector3 _rot, class CWorld* world) :
	CObject(glm::vec3(0), glm::vec3(0)), m_camera(glm::vec3(0), glm::vec3(0)), m_camerayrot(0), m_collider({ -0.30, 0, -0.30 }, { 0.30, 1.75, 0.30 }), m_world(world), m_velocity(0)
{
	m_camera.Attach(this);
	m_camera.SetPosition({ 0, 1.65, 0 });
	m_camera.MakePerspective(90.f, 4.f / 3.f, 0.1f, 500.f);

	m_flags = 0;

	m_collider.getCollider().SetPosition(_pos);
	Attach(&m_collider.getCollider());
}

bool CFPPlayer::checkFlag(const uint8_t& flag) const
{
	return (m_flags & flag);
}

void CFPPlayer::setFlags(const uint8_t& flag, const bool& set)
{
	m_flags = (set ? (m_flags | flag) : (m_flags & ~flag));
}

void CFPPlayer::RotateCamera(glm::ivec2 pos, float sensivity, float delta)
{
	glm::vec2 tpos = pos;
	tpos *= sensivity* delta;

	//CAMERA
	{
		SVector3 temp = m_camera.GetGlobalRotation();
		m_camerayrot = clamp(m_camerayrot + tpos.y, -89.9f, 89.9f);
		temp.x = m_camerayrot;
		m_camera.SetGlobalRotation(temp);
	}
	//PLAYER
	{
		SVector3 temp1 = GetGlobalRotation();
		temp1.y += tpos.x;
		SetGlobalRotation(temp1);
	}
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void CFPPlayer::Move(bool forward, bool back, bool top, bool bottom, bool left, bool right, float delta, float gravity)
{
	if (!m_world) return;

	glm::mat4 matrix = getModelMatrix();

	glm::vec3 vforward = -glm::normalize(matrix[2]);
	glm::vec3 vright = glm::normalize(matrix[0]);
	glm::vec3 vtop = glm::vec3(0, 1, 0);


	glm::mat4 invcam = glm::inverse(m_camera.getModelMatrix());
	glm::vec3 ray = glm::vec3(invcam[2]) * glm::vec3(1, 1, -1), position, normal;
	ray *= 6.f;
	if (m_collider.RayCast(m_camera.GetGlobalPosition(), ray, position, normal, &m_world->getManager()))
	{
		if (!m_targetBlock.isInit())
		{
			float epsilon = 10e-4;
			m_targetBlock.InitDebugRect(-0.5 - epsilon, -0.5 - epsilon, -0.5-epsilon, 0.5+ epsilon, 0.5 + epsilon, 0.5 + epsilon);
			m_targetBlock.setColor(glm::vec3(0));
		}
		if(!m_debugTarget.isInit())
		{
			m_debugTarget.InitCube(0.2);
			m_debugTarget.setColor(glm::vec3(1, 0, 0));
		}

		m_pointing = glm::floor(position);
	
		if (normal.x > 0)
			m_pointing.x--;
		if (normal.z > 0)
			m_pointing.z--;
		if (normal.y > 0)
			m_pointing.y--;


		//m_targetBlock.SetPosition({ 0, -100000, 0 });
		m_targetBlock.SetPosition(glm::vec3(0.5)+ m_pointing);
		m_debugTarget.SetPosition(glm::vec3(position));

	}
	else
	{
		m_targetBlock.SetPosition({ 0, -100000, 0 });
	}

	if (checkFlag(IS_HOVERING))
	{
		m_velocity = (vforward * (float)(forward - back) + vright * (float)(right - left) + vtop * (float)(top - bottom)) * delta * m_speed;
		m_velocity = glm::normalize(m_velocity) * delta * m_speed;
		if (glm::all(glm::isnan(m_velocity)))
			m_velocity = glm::vec3(0);
		if(!checkFlag(IS_NOCLIPPING)) 
			m_collider.Move(m_velocity, &m_world->getManager());
		else
			m_collider.getCollider().Move(m_velocity);
	}
	else
	{
		//float y = m_accel.y;

		m_accel = (vforward * (float)(forward - back) + vright * (float)(right - left)) * m_speed;
		m_accel.y = -9.81;
		m_accel *= delta;


		for (int i = 0; i < 3; ++i)
		{
			if (i != 1)
				m_velocity[i] = glm::clamp(m_velocity[i] + m_accel[i] * 3 * delta, std::min(0.f, m_accel[i]), std::max(0.f, m_accel[i]));
			else
				m_velocity[i] += m_accel[i] * delta;
			m_accel *= 1 - delta;
		}

		m_collider.Move(m_velocity, &m_world->getManager());
		float y = m_velocity.y;

		m_velocity = (vforward * (float)(forward - back) + vright * (float)(right - left));
		m_velocity = glm::normalize(m_velocity) * delta * m_speed * m_rampUp;
		m_velocity.y = y;

		if (std::isnan(m_velocity.x) || std::isnan(m_velocity.z)) m_velocity = glm::vec3(0, y, 0);

		if (forward || back || right || left)
			m_rampUp = std::min(delta / 0.25f + m_rampUp, 1.f);
		else 
			m_rampUp = std::max(m_rampUp - delta / 0.25f, 0.f);

		if (glm::all(glm::isnan(m_velocity)))
			m_velocity = glm::vec3(0);

		if (checkFlag(IS_JUMPING))
		{
			float old = jumpHyperbola(m_jumpProgress);

			m_jumpProgress += delta / m_jumpDuration;
			if (m_jumpProgress > 1)
			{
				setFlags(IS_JUMPING, 0);
				m_jumpProgress = 0;
				m_jumpCoolDown = m_jumpCoolDownMax/4.f;
				//m_velocity += vtop * m_gravity * delta;
			}

			float speed = jumpHyperbola(m_jumpProgress) - old + m_jumpHeight*delta*delta;
			m_velocity.y = speed;
		}
		else
			m_velocity += vtop * m_gravity * delta * 0.05f;

		uint8_t normals = m_collider.Move(m_velocity, &m_world->getManager());
		
		if (top && (normals & EntityC_Up) && m_jumpCoolDown <= 0)
		{
			setFlags(IS_JUMPING, 1);
			m_velocity.y = 0;
		}
		else
			m_jumpCoolDown -= delta;


		if (checkFlag(IS_JUMPING) && (normals & EntityC_Down))
		{
			setFlags(IS_JUMPING, 0);
			m_jumpProgress = 0;
			m_jumpCoolDown = m_jumpCoolDownMax;
		}

	}
}

CCamera& CFPPlayer::getCamera()
{
	return m_camera;
}

#include "../CFrustumCollider.h"
void CFPPlayer::Tick(CInputManager& _manager, float deltaTime)
{
	RotateCamera(-_manager.getMouseRelative(), 0.2f, deltaTime * 1000*m_sensivity);

	if (_manager.keyDown(SDL_SCANCODE_Q))
		m_speed = 500;
	else
		m_speed = 5;
	static bool key1Down = false;
	if(_manager.keyDown(SDL_SCANCODE_1))
	{
		if(!key1Down)
			setFlags(IS_HOVERING, !checkFlag(IS_HOVERING));
		key1Down = true;
	}
	else
		key1Down = false;

	static bool key2Down = false;
	if(_manager.keyDown(SDL_SCANCODE_2))
	{
		if(!key2Down)
			setFlags(IS_NOCLIPPING, !checkFlag(IS_NOCLIPPING));
			//m_noclip = !m_noclip;
		key2Down = true;
	}
	else
		key2Down = false;

	static int pressed = false;
	if (_manager.mouseButtonDown(LEFT_MOUSE))
	{
		//std::cout << (int)m_world->setBlock(m_pointing.x, m_pointing.y, m_pointing.z, BLOCK_AIR);
		if (!pressed)
		{
			pressed = true;
			glm::vec3 pos = glm::floor(m_pointing);
			m_world->setBlock(pos.x, pos.y, pos.z, BLOCK_AIR);
		}
	}
	else pressed = false;

	static int pressed2 = false;
	if (_manager.keyDown(SDL_SCANCODE_C))
	{
		//std::cout << (int)m_world->setBlock(m_pointing.x, m_pointing.y, m_pointing.z, BLOCK_AIR);
		if (!pressed2)
		{
			pressed2 = true;
			glm::vec3 pos = glm::floor(GetGlobalPosition()/(float)CHUNK_SIZE);
			CChunkPart* part = m_world->getManager().getChunkPart(pos.x, pos.z);
			if (part)
			{
				CChunk* chunk = part->getChunk(pos.y);
				chunk->generateCollider(true);
			}
		}
	}
	else pressed2 = false;


	static int pressed3 = false;
	if (_manager.keyDown(SDL_SCANCODE_V))
	{
		//std::cout << (int)m_world->setBlock(m_pointing.x, m_pointing.y, m_pointing.z, BLOCK_AIR);
		if (!pressed3)
		{
			pressed2 = true;
			glm::vec3 pos = glm::floor(GetGlobalPosition() / (float)CHUNK_SIZE);
			CChunkPart* part = m_world->getManager().getChunkPart(pos.x, pos.z);
			if (part)
			{
				CChunk* chunk = part->getChunk(pos.y);
				chunk->emptyCollider();
			}
		}
	}
	else pressed3 = false;

	//if (_manager.keyDown(SDL_SCANCODE_C))
	//	m_collider.forceRegen(m_collider.getCollider().GetPosition() / 16.f, m_manager);

	Move(_manager.keyDown(SDL_SCANCODE_W),
		 _manager.keyDown(SDL_SCANCODE_S),
		 _manager.keyDown(SDL_SCANCODE_SPACE),
		 _manager.keyDown(SDL_SCANCODE_LSHIFT),
		 _manager.keyDown(SDL_SCANCODE_A),
		 _manager.keyDown(SDL_SCANCODE_D),
		 deltaTime,
		-20.f);


	//m_collider.applyCollision(m_testcollider, 0.f);

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void CFPPlayer::Draw(const SDrawInfo& info)
{
	m_collider.getCollider().Draw(info);
	if(info.testlight != nullptr)
		m_targetBlock.Draw(info);
}

void CFPPlayer::BulkDraw(class CBulkRenderer* renderer)
{
	m_collider.getCollider().BulkDraw(renderer);
	m_targetBlock.BulkDraw(renderer);
	m_debugTarget.BulkDraw(renderer);
}

void CFPPlayer::setWorld(class CWorld* world)
{
	m_world = world;
}

void CFPPlayer::Hover(bool value)
{
	setFlags(IS_HOVERING, value);
}

bool CFPPlayer::isJumping()
{
	return checkFlag(IS_JUMPING);
}

bool CFPPlayer::canJump()
{
	return false;
}
