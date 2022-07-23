#include "../CFrustumCollider.h"
#include "../CFileManager.h"
#include "../Game/CWorld.h"
#include "CFPPlayer.h"
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

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

	m_flags = IS_HOVERING;
	m_holdingBlock = BLOCK_DIRT;
	m_collider.getCollider().SetPosition(_pos);
	Attach(&m_collider.getCollider());
}

CFPPlayer::~CFPPlayer()
{
	save(m_world->getFilePath());
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
	glm::mat4 matrix = getModelMatrix(nullptr);

	glm::vec3 vforward = -glm::normalize(matrix[2]);
	glm::vec3 vright = glm::normalize(matrix[0]);
	glm::vec3 vtop = glm::vec3(0, 1, 0);


	glm::mat4 invcam = glm::inverse(m_camera.getModelMatrix(nullptr));
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

		m_targetPlace = m_pointing + normal;

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


void CFPPlayer::placeBlock(int block)
{
	CAABB box(m_targetPlace, m_targetPlace+glm::ivec3(1));
	if(m_collider.getCollider().checkIntersetion(box) == AABB_NotIntersected)
	{
		m_world->setBlock(m_targetPlace.x, m_targetPlace.y, m_targetPlace.z, block);
	}
}

void CFPPlayer::Tick(CInputManager& _manager, float deltaTime)
{
	RotateCamera(-_manager.getMouseRelative(), 0.2f, deltaTime * 1000 * m_sensivity);

	if (_manager.keyDown(SDL_SCANCODE_Q))
		m_speed = 500;
	else
		m_speed = 5;
	if(_manager.keyPressed(SDL_SCANCODE_1))
	{
		setFlags(IS_HOVERING, !checkFlag(IS_HOVERING));
	}
	if(_manager.keyPressed(SDL_SCANCODE_2))
	{
		setFlags(IS_NOCLIPPING, !checkFlag(IS_NOCLIPPING));
	}
	m_lastPlaced += deltaTime;

	if (_manager.mouseButtonDown(LEFT_MOUSE) && m_lastPlaced > 0.2)
	{
		m_lastPlaced = 0;
		glm::vec3 pos = glm::floor(m_pointing);
		m_world->setBlock(pos.x, pos.y, pos.z, BLOCK_AIR);
	}
	
	if (_manager.mouseButtonDown(RIGHT_MOUSE) && m_lastPlaced > 0.2)
	{
		m_lastPlaced = 0;
		placeBlock(m_holdingBlock);
	}

	bool left_arrow = _manager.keyPressed(SDL_SCANCODE_LEFT);
	bool right_arrow = _manager.keyPressed(SDL_SCANCODE_RIGHT);
	if (left_arrow || right_arrow)
	{
		if(right_arrow)
			++m_holdingBlock;
		else
			--m_holdingBlock;
		if(m_holdingBlock >= BLOCK_TOTAL)
			m_holdingBlock %= BLOCK_TOTAL;
		if(m_holdingBlock <= 0)
			m_holdingBlock = BLOCK_TOTAL-1;

		//m_holdingBlock = right_arrow ? (m_holdingBlock + 1) % (BLOCK_TOTAL-1)+1 : (m_holdingBlock - 1 <= 0 ? 1 : m_holdingBlock - 1);
		if(m_holdingBlockMesh)
		{
			m_holdingBlockMesh->Clear();
			CBlock* faces[6]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
			m_holdingBlockMesh->Init(BLOCK_DATABASE::getBlock(m_holdingBlock)->getBlockMeshVertices(faces, m_world->getAtlas()));
		}
	}

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
	//m_collider.getCollider().BulkDraw(renderer);
	m_targetBlock.BulkDraw(renderer);
	//m_debugTarget.BulkDraw(renderer);
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

void CFPPlayer::save(const std::string& path)
{
	if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) { 
        std::filesystem::create_directory(path);
    }
	std::string pth = path + "/player.data";
	std::ofstream file(pth, std::ios::binary | std::ofstream::trunc);
	SVector3 position = m_collider.getCollider().GetPosition(), rotation = m_collider.getCollider().GetRotation();
	file.write((char*)&position, sizeof(position));
	file.write((char*)&rotation, sizeof(rotation));
	file.close();
}

void CFPPlayer::load(const std::string& path)
{
	if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) { 
        return;
    }
	std::string pth = path + "/player.data";
	std::ifstream file(pth, std::ios::binary);
	if(!file.is_open())
		return;
	SVector3 position, rotation;
	file.read((char*)&position, sizeof(SVector3));
	file.read((char*)&rotation, sizeof(SVector3));
	m_collider.getCollider().SetPosition(position);
	m_collider.getCollider().SetRotation(rotation);
	file.close();
}

void CFPPlayer::setHoldingBlockMesh(CMesh* mesh)
{
	m_holdingBlockMesh = mesh;
	CBlock* faces[6]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	m_holdingBlockMesh->Init(BLOCK_DATABASE::getBlock(m_holdingBlock)->getBlockMeshVertices(faces, m_world->getAtlas()));
	m_holdingBlockMesh->SetTexture(m_world->getAtlas());
}