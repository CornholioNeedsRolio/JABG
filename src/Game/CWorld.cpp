#include "CWorld.h"
#include "../CFileManager.h"
#include <iostream>


CWorld::CWorld(std::shared_ptr<CTextureAtlas> texture, CFPPlayer& player) :
	m_texture(texture), m_manager(this), m_threadManager(this), m_player(&player), m_playerPos(0,0,0), m_loader(2, this)
{
	m_shader = CFileManager::getShader("./res/DefaultShader/DefaultShader");
}

CWorld::~CWorld(){}

uint8_t CWorld::setBlock(int x, int y, int z, int id)
{	
	auto position = CChunkManager::getLocal(x, y, z);
	int cx = std::get<0>(position.first);
	int cy = std::get<1>(position.first);
	int cz = std::get<2>(position.first);


	CChunkPart* part = m_manager.getChunkPart(cx, cz);
	if (!part)
		return SET_INVALIDPART;

	CChunk* chunk = part->getChunk(cy);
	if (chunk)
	{
		int x = std::get<0>(position.second);
		int y = std::get<1>(position.second);
		int z = std::get<2>(position.second);


		return chunk->SetBlock(x, y, z, id);;
	}
	else
		return SET_INVALIDCHUNK;
}

int CWorld::getBlock(int x, int y, int z)
{
	auto position = CChunkManager::getLocal(x, y, z);
	CChunkPart* part = m_manager.getChunkPart(std::get<0>(position.first), std::get<2>(position.first));
	if (!part)
		return 0;

	CChunk* chunk = part->getChunk(std::get<1>(position.first));
	if (chunk)
		return chunk->GetBlock(std::get<0>(position.second), std::get<1>(position.second), std::get<2>(position.second));

	return 0;
}

CChunkManager& CWorld::getManager()
{
	return m_manager;
}

/*CTerrainGenerator& CWorld::getGenerator()
{
	return m_terrainGenerator;
}*/

void CWorld::Draw(const SDrawInfo& info)
{
	m_manager.Draw(info);
}

void CWorld::BulkDraw(class CBulkRenderer* renderer)
{
	m_manager.BulkDraw(renderer);
}

void CWorld::Tick(CInputManager& _manager, float deltaTime)
{
	glm::ivec3 playerPos = m_player->GetGlobalPosition();
	auto position = CChunkManager::getLocal(playerPos.x, playerPos.y, playerPos.z);

	m_loader.Tick(&m_manager);
	if (position.first != m_playerPos || !(m_flags & NOT_FIRST_RUN))
	{
		m_playerPos = position.first;
		m_flags |= NOT_FIRST_RUN;

		//m_terrainGenerator.generateWorld(m_playerPos, getDrawDistance(), &m_manager);
		m_threadManager.setPlayerPos(m_playerPos, getDrawDistance());
		m_loader.onPlayerMove(m_playerPos, getDrawDistance(), &m_manager);

		m_manager.Clear(m_playerPos);
	}
	m_threadManager.ChooseChunk();
}

int CWorld::getDrawDistance()
{
	return 16;
}

std::shared_ptr<CTextureAtlas> CWorld::getAtlas()
{
	return m_texture;
}

std::shared_ptr<CShader> CWorld::getShader()
{
	return m_shader;
}
