#include "CChunk.h"
#include "CChunkCollider.h"
#include "CChunkPart.h"
#include "CChunkManager.h"
#include <iostream>
#include "CWorld.h"
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include "../CFrustumCollider.h"
#include "../BulkRenderer/CBulkRenderer.h"


std::unique_ptr<CMesh> CChunk::m_bounds = 0x0;

CBlock* CChunk::getBlockPtr(int x, int y, int z) 
{
	return BLOCK_DATABASE::getBlock(GetBlock(x, y, z));
}

CChunk::CChunk(glm::ivec3 position, CChunkManager* manager) :
	m_position(position), m_manager(manager)
{
	m_flags = 0;
	m_blocks = 0x0;
	m_nflags = 0;
	m_collider = 0x0;
	//MakeCollider();

	//m_nflags |= NORMAL_DEBUG;
	//m_nflags |= GENERATE_COLLIDER;
}

void CChunk::setManager(class CChunkManager* manager)
{
	m_manager = manager;
	makeNeighborsDirty();
}

CChunk::CChunk(CChunk&& A)
{
	m_position = A.m_position;

	if (A.m_blocks)
		m_blocks = std::move(A.m_blocks);
	else
		m_blocks = 0x0;

	m_manager = A.m_manager;

	m_nflags = A.m_nflags;
	m_flags.store(A.m_flags);
	m_collider = 0x0;// std::move(A.m_collider);
}

CChunk::~CChunk()
{
	if(m_collider)
		delete m_collider;
}

void CChunk::setTextureAndShader(std::shared_ptr<CTextureAtlas> texture, std::shared_ptr<CShader> shader)
{
	m_mesh.SetTexture(texture);
	m_mesh.SetShader(shader);

	if (m_bounds)
		m_bounds->SetShader(shader);
}

blocktype CChunk::GetBlock(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
	{
		std::cout << "[Chunk]Requested data outside the chunk range" << " x: " << x << " y: " << y << " z: " << z << '\n';
		return 0;
	}
	if (!m_blocks) 
		return 0;

	//if something requests a block before the mesh build is killed we extract from the queue
	if (m_flags & THREAD_RUNNING)
	{
		auto it = m_edits.find(std::make_tuple(x, y, z));
		if (it != m_edits.end())
			return it->second;
	}
	else consumeEdits();

	return m_blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

uint8_t CChunk::SetBlock(int x, int y, int z, blocktype id)
{
	if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
	{
		std::cout << "[Chunk]Attempted to set data outside chunk range" << x << ' ' << y << ' ' << z << '\n';
		return SET_OUTOFBOUNDS;
	}
	uint8_t output = SET_SUCCESFUL;
	if (GetBlock(x, y, z) == id)
		return SET_SAMETYPE;

	//if (!m_collider) m_collider = std::make_shared<CChunkCollider>(m_position * CHUNK_SIZE, glm::vec3(CHUNK_SIZE));


	if (x == 0)
		makeNeighborDirty(CARD_S);
	if (x == CHUNK_SIZE - 1)
		makeNeighborDirty(CARD_N);
	if (z == 0)
		makeNeighborDirty(CARD_W);
	if (z == CHUNK_SIZE - 1)
		makeNeighborDirty(CARD_E);
	
	if (y == 0)
		makeNeighborDirty(CARD_DOWN);
	if (y == CHUNK_SIZE - 1)
		makeNeighborDirty(CARD_UP);

	//If thread is running we kill it, and we add this to the queue
	if (m_flags & THREAD_RUNNING)
	{
		auto it = m_edits.find(std::make_tuple(x, y, z));
		if (it != m_edits.end())
			it->second = id;
		else
			m_edits.emplace(std::make_tuple(x, y, z), id);

		m_flags |= KILL_THREAD;
		output = SET_THREADRUNNING;
	}
	else
	{
		if (!m_blocks)
		{
			m_blocks = std::make_unique<blocktype[]>(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
		}

		consumeEdits();
		MakeDirty();//mark the chunk for mesh rebuilding


		setBlockInCollider(x, y, z, id);
		m_blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = id;
	}
	return output;
}

void CChunk::Draw(const SDrawInfo& info)
{
	/*CFrustumCollider *collider = info.camera->getCollider();
	if (collider)
	{
		if (!collider->sphereIntersection(m_position * CHUNK_SIZE + (int)(CHUNK_SIZE * 0.5), CHUNK_SIZE*1.25))
			return;
	}*/

	if(!info.camera->getCollider().sphereIntersection((m_position * CHUNK_SIZE + (int)(CHUNK_SIZE * 0.5)), (float)CHUNK_SIZE*1.25))
		return;
	if(info.testlight == nullptr)
		if(m_bounds)
			m_bounds->setColor(glm::vec3(1, 1, 1));
			
	if ((m_nflags & NORMAL_DEBUG) && info.testlight != nullptr)
	{
		if (!m_bounds)
		{
			m_bounds = std::make_unique<CMesh>();
			m_bounds->SetShader(m_mesh.GetShader());
			m_bounds->InitDebugRect(0,0,0,CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
			goto Init;
		}
		
		if (!m_bounds->isInit())
		{
			Init:
			m_bounds->setColor(glm::vec3(0, 0, 0));	
		}
		//if(info.testlight == nullptr)

		m_bounds->SetPosition(m_position * CHUNK_SIZE);
		m_bounds->Draw(info);

	}

	if (m_flags & THREAD_COMPLETED && (m_flags & THREAD_RUNNING) == 0)
	{
		m_mesh.Clear();

		if (!m_meshinfo.empty())
		{
			m_mesh.Init(m_meshinfo);
			m_meshinfo.clear();
		}
		m_flags &= ~THREAD_COMPLETED;
	}
	if (!m_mesh.isInit())
		return;

	//if(m_collider)
	//   m_collider->Draw(info);

	m_mesh.SetPosition(m_position * CHUNK_SIZE);
	m_mesh.Draw(info);
}

bool CChunk::fustumCulling(class CCamera* camera) const
{
	if(!camera) return true;
	return camera->getCollider().sphereIntersection((m_position * CHUNK_SIZE + (int)(CHUNK_SIZE * 0.5)), (float)CHUNK_SIZE*1.25);
}

void CChunk::setAllBlocks(std::unique_ptr<blocktype[]> blocks)
{
	m_blocks = std::move(blocks);
	generateCollider();
	MakeDirty();
}

void CChunk::BulkDraw(class CBulkRenderer* renderer) 
{
	if (m_flags & THREAD_COMPLETED && (m_flags & THREAD_RUNNING) == 0)
	{
		m_mesh.Clear();

		if (!m_meshinfo.empty())
		{
			m_mesh.Init(m_meshinfo);
			m_meshinfo.clear();
		}
		m_flags &= ~THREAD_COMPLETED;
	}
	
	if (!m_mesh.isInit())
		return;
	CCamera* camera = renderer->getActiveCamera();
	if(!fustumCulling(camera)) 
		return;
	m_mesh.SetPosition(m_position * CHUNK_SIZE);
	m_mesh.BulkDraw(renderer);
}

void CChunk::Tick(CInputManager& _manager, float deltaTime)
{
	if (!(m_nflags & THREAD_RUNNING))
		consumeEdits();
}

void CChunk::BuildMesh(std::array<CChunk*, 6> neighbors, std::shared_ptr<CTextureAtlas> atlas)
{
	auto getBlock = [&](int x, int y, int z)
	{
		if (z == -1 && neighbors[CHUNKFACE_BACK])
			return neighbors[CHUNKFACE_BACK]->GetBlock(x, y, CHUNK_SIZE-1);
		if (z == CHUNK_SIZE && neighbors[CHUNKFACE_FRONT])
			return neighbors[CHUNKFACE_FRONT]->GetBlock(x, y, 0);

		if (x == -1 && neighbors[CHUNKFACE_LEFT])
			return neighbors[CHUNKFACE_LEFT]->GetBlock(CHUNK_SIZE - 1, y, z);
		if (x == CHUNK_SIZE && neighbors[CHUNKFACE_RIGHT])
			return neighbors[CHUNKFACE_RIGHT]->GetBlock(0, y, z);

		if (y == -1 && neighbors[CHUNKFACE_BOT])
			return neighbors[CHUNKFACE_BOT]->GetBlock(x, CHUNK_SIZE - 1, z);
		if (y == CHUNK_SIZE && neighbors[CHUNKFACE_TOP])
			return neighbors[CHUNKFACE_TOP]->GetBlock(x, 0, z);

		/*
			*TODO: make it fetch from neighbors, in a safe manner 
		*/
		if (z == -1 && neighbors[CHUNKFACE_BACK])
			return neighbors[CHUNKFACE_BACK]->GetBlock(x, y, z);
		
		if (x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE)
			return m_blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
		return blocktype(0);
	};

	while (m_nflags & CONSUME_EDITS);

	m_flags &= ~THREAD_COMPLETED; //Setting completed to false
	m_flags |= THREAD_RUNNING; //Setting running to running

	if (m_flags & GENERATING_CHUNK)
	{
		m_flags &= ~THREAD_RUNNING; //Setting thread running to false
		return;
	}

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if ((m_flags & KILL_THREAD) || (m_flags & GENERATING_CHUNK))
				{
					//std::cout << " :P";
					m_flags &= ~KILL_THREAD; //Setting kill_thread to false
					m_flags &= ~THREAD_RUNNING; //Setting thread running to false
					return;
				}

				if (!BLOCK_DATABASE::getBlock(getBlock(x, y, z))->isVisible())
					continue;

				bool temp[6] = {};

				temp[CBlock::BLOCK_BACK]   = !BLOCK_DATABASE::getBlock(getBlock(x, y, z-1))->isVisible();
				temp[CBlock::BLOCK_FRONT]  = !BLOCK_DATABASE::getBlock(getBlock(x, y, z+1))->isVisible();
				temp[CBlock::BLOCK_RIGHT]  = !BLOCK_DATABASE::getBlock(getBlock(x+1, y, z))->isVisible();
				temp[CBlock::BLOCK_LEFT]   = !BLOCK_DATABASE::getBlock(getBlock(x-1, y, z))->isVisible();
				temp[CBlock::BLOCK_TOP]    = !BLOCK_DATABASE::getBlock(getBlock(x, y+1, z))->isVisible();
				temp[CBlock::BLOCK_BOTTOM] = !BLOCK_DATABASE::getBlock(getBlock(x, y-1, z))->isVisible();

				auto blockmeshinfo = BLOCK_DATABASE::getBlock(GetBlock(x, y, z))->getBlockMeshVertices(temp, atlas, {x, y, z});

				m_meshinfo.insert(m_meshinfo.end(), blockmeshinfo.begin(), blockmeshinfo.end());
			}
		}
	}
	m_flags &= ~THREAD_RUNNING;
	m_flags &= ~CHUNK_DIRTY;
	m_flags |= THREAD_COMPLETED;

}

const glm::ivec3& CChunk::getChunkPosition() const
{
	return m_position;
}

bool CChunk::isChunkDirty()
{
	return (m_flags & CHUNK_DIRTY) && m_blocks;
}

void CChunk::MakeDirty()
{
	m_flags |= CHUNK_DIRTY;
	if (m_flags & THREAD_RUNNING)
		m_flags |= KILL_THREAD;
}

void CChunk::makeNeighborDirty(uint8_t card)
{
	if (!m_manager) return;
	CChunkPart* chunkpart = 0x0;
	int cx = m_position.x;
	int cz = m_position.z;
	int cy = m_position.y; 
	switch (card)
	{
	case CARD_S: //x==0
		chunkpart = m_manager->getChunkPart(cx - 1, cz);
		break;
	case CARD_N://x==16
		chunkpart = m_manager->getChunkPart(cx + 1, cz);
		break;
	case CARD_W://z==0
		chunkpart = m_manager->getChunkPart(cx, cz - 1);
		break;
	case CARD_E://z==16
		chunkpart = m_manager->getChunkPart(cx, cz + 1);
		break;
	default:
		chunkpart = m_manager->getChunkPart(cx, cz);
		break;
	}
	if (chunkpart)
	{
		CChunk* chunk = 0x0;
		if (card == CARD_UP)
			chunk = chunkpart->getChunk(cy + 1);
		else if(card == CARD_DOWN)
			chunk = chunkpart->getChunk(cy - 1);
		else
			chunk = chunkpart->getChunk(cy);

		if (chunk)
			chunk->MakeDirty();
	}
}

void CChunk::makeNeighborsDirty()
{
	if (!m_manager) return;

	for(int i = 0; i < 5; ++i)
		makeNeighborDirty(i);
	return;

	int cx = m_position.x;
	int cz = m_position.z;
	int cy = m_position.y;
	std::vector<CChunk*> m_neighbours;
	CChunkPart* chunkpart[] = {
		m_manager->getChunkPart(cx - 1, cz),
		m_manager->getChunkPart(cx + 1, cz),
		m_manager->getChunkPart(cx, cz - 1),
		m_manager->getChunkPart(cx, cz + 1)
	};
	std::cout << 1 << std::flush;
	for(int i = 4; i; --i) 
	{
		if(chunkpart[i] == nullptr) continue;
		CChunk* chunk = chunkpart[i]->getChunk(cy);
		m_neighbours.push_back(chunk);
	}
	std::cout << 2 << std::flush;
	CChunkPart* parent = m_manager->getChunkPart(cx, cz);
	if(parent)
	{
		m_neighbours.push_back(parent->getChunk(cy + 1));
		m_neighbours.push_back(parent->getChunk(cy - 1));
	}
	std::cout << 3 << std::endl;
	for(CChunk* chunk : m_neighbours)
		if(chunk) chunk->MakeDirty();
}

bool CChunk::consumeEdits()
{
	if (!m_edits.empty() && !(m_flags & CONSUME_EDITS))
	{
		//if the thread is not running then I consume everything from the queue

		for (auto it = m_edits.begin(); it != m_edits.end(); it++)
		{
			int x = std::get<0>(it->first), y = std::get<1>(it->first), z = std::get<2>(it->first);

			setBlockInCollider(x, y, z, it->second);
			m_blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = it->second;
		}
		m_edits.clear();
		m_flags &= ~CONSUME_EDITS;
		MakeDirty();
		return true;
	}
	return false;
}

bool CChunk::isChunkGenerating()
{
	return m_flags & GENERATING_CHUNK;
}

void CChunk::setChunkGenerating(bool value)
{
	if (!value)
		m_flags &= ~GENERATING_CHUNK;
	else
		m_flags |= GENERATING_CHUNK;
}

bool CChunk::canDestroy()
{
	return (m_flags & (IMPORTANT_NEIGHBOR | THREAD_RUNNING)) == 0;
}

void CChunk::setDestroyable(bool destroyable)
{
	if (destroyable)
		m_flags &= ~IMPORTANT_NEIGHBOR;
	else
		m_flags |= IMPORTANT_NEIGHBOR;
	//m_flags = !destroyable ? (m_flags | IMPORTANT_NEIGHBOR) : (m_flags & ~IMPORTANT_NEIGHBOR);
}

bool CChunk::intersectsCollider(CAABB* other, const glm::vec3& velocity)
{
	if (!m_collider)
		MakeCollider();

	glm::vec3 normal; float delta; bool inside, output = other->RaySweep(m_collider->getCollider(), velocity, normal, delta, &inside);
	return output || inside;
		
}

void CChunk::generateCollider(bool force)
{
	if (!m_collider || !(m_nflags & GENERATE_COLLIDER) || force)
	{
		MakeCollider();

		m_nflags |= GENERATE_COLLIDER;

		for (int x = 0; x < CHUNK_SIZE; ++x)
		for (int y = 0; y < CHUNK_SIZE; ++y)
		for (int z = 0; z < CHUNK_SIZE; ++z)
			setBlockInCollider(x, y, z, GetBlock(x, y, z));
	}

}

void CChunk::emptyCollider()
{
	if (m_collider && (m_nflags & GENERATE_COLLIDER))
	{
		//MakeCollider();

		//m_nflags |= GENERATE_COLLIDER;

		for (int x = 0; x < CHUNK_SIZE; ++x)
			for (int y = 0; y < CHUNK_SIZE; ++y)
				for (int z = 0; z < CHUNK_SIZE; ++z)
					setBlockInCollider(x, y, z, 0);
	}
}

void CChunk::MakeCollider()
{
	//m_collider = std::make_unique<CChunkCollider>(m_position * CHUNK_SIZE, glm::vec3(CHUNK_SIZE));
	if(m_collider)
		delete m_collider;

	m_collider = new CChunkCollider(m_position * CHUNK_SIZE, glm::vec3(CHUNK_SIZE));
}

void CChunk::setBlockInCollider(int x, int y, int z, blocktype id)
{
	if(m_collider && (m_nflags & GENERATE_COLLIDER))
		m_collider->setInfo(m_position.x * CHUNK_SIZE + x, m_position.y * CHUNK_SIZE + y, m_position.z * CHUNK_SIZE + z, BLOCK_DATABASE::getBlock(id)->getCollision());
}

CChunkCollider* CChunk::getCollider()
{
	return m_collider;
}

CChunk& CChunk::operator=(const CChunk& A)
{
	this->m_position = A.m_position;
	//this->m_blocks = A.m_blocks;
	return *this;
}