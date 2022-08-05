#include "CChunkManager.h"
#include "Game/CWorld.h"
#include <iostream>

std::pair<UChunkPos, ULocalPos> CChunkManager::getLocal(int x, int y, int z)
{
	int cx = std::floor((float)x / (float)CHUNK_SIZE);
	int cy = std::floor((float)y / (float)CHUNK_SIZE);
	int cz = std::floor((float)z / (float)CHUNK_SIZE);

	int bx = x % CHUNK_SIZE;
	if (bx < 0) bx += CHUNK_SIZE;
	int by = y % CHUNK_SIZE;
	if (by < 0) by += CHUNK_SIZE;
	int bz = z % CHUNK_SIZE;
	if (bz < 0) bz += CHUNK_SIZE;

	//std::cout << cx << ' ' << cy << ' ' << cz << ' ' << bx << ' ' << by << ' ' << bz  << ' ' << x << ' ' << y << ' ' << z << '\n';

	return {{ cx, cy, cz }, { bx, by, bz}};
}

CChunkManager::CChunkManager(CWorld* world) : 
	m_world(world)
{

}

CWorld* CChunkManager::getWorld()
{
	return m_world;
}

CChunkPart* CChunkManager::getChunkPart(int x, int z)
{
	auto it = m_chunks.find(std::make_pair(x, z));
	if (it != m_chunks.end()) return &it->second;
	return nullptr;
}

CChunkPart* CChunkManager::createChunkIfNone(int x, int z, bool* existed)
{
	auto it = m_chunks.find(UChunkPartPos(x, z));
	if(it == m_chunks.end())
	{
		m_chunks.insert(std::make_pair<UChunkPartPos, CChunkPart>(UChunkPartPos(x, z), CChunkPart(m_world, {x, z})));
		it = m_chunks.find(UChunkPartPos(x, z));
	}

	return &it->second;
}

CChunkPart* CChunkManager::createChunkIfNoneGenerated(int x, int z)
{
	bool existed;
	CChunkPart* part = createChunkIfNone(x, z, &existed);
	//if (!existed)
	//	m_world->getGenerator().makeChunkPart(x, z, part);
	return part;
}

std::map<UChunkPartPos, CChunkPart>::iterator CChunkManager::getBeginIterator()
{
	return m_chunks.begin();
}

std::map<UChunkPartPos, CChunkPart>::iterator CChunkManager::getEndIterator()
{
	return m_chunks.end();
}

void CChunkManager::Draw(const SDrawInfo& info)
{
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) 
	{
		it->second.Draw(info);
		// it->second.Draw(info);
	}
}

void CChunkManager::BulkDraw(class CBulkRenderer* renderer)
{
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) 
	{
		it->second.BulkDraw(renderer);
		// it->second.Draw(info);
	}
}

void CChunkManager::Clear(const std::tuple<int,int,int>& playerPos)
{
	int cx = std::get<0>(playerPos);
	int cy = std::get<1>(playerPos);
	int cz = std::get<2>(playerPos);
	for (auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		UChunkPartPos position = it->first;
		int rd = m_world->getDrawDistance()*0.5+4;
		if ((position.first - rd >= cx || cx >= position.first + rd || position.second - rd >= cz || cz >= position.second + rd))
		{
			it = m_chunks.erase(it);
		}
		else 
		{
			it->second.UnloadChunks(cy, rd);
			it++;
		}
	}
}
