#ifndef CTERRAINGENERATOR_HEADER
#define CTERRAINGENERATOR_HEADER
#include <glm/glm.hpp>
#include <array>
#include <map>
#include "CStructureTree.h"
#include "Game/Chunk/CChunk.h"

/*
struct comp
{
	bool operator()(const glm::ivec3& lhs, const glm::ivec3& rhs) const
	{
		return lhs.x < rhs.x || lhs.x == rhs.x && (lhs.y < rhs.y || lhs.y == rhs.y && lhs.z < rhs.z);
	};
};*/


///// DEPRECATED, OLD GENERATOR
class CTerrainGenerator
{
	int m_maxy = 5;
	bool NeedTree(int x, int z);
	float OctaveSimplex(float x, float y, float z, int octaves, float persistence);

	std::map<std::pair<int, int>, float> m_heightMap;

	StructureInfo m_cache;
	CStructureTree m_tree;
public:
	void makeChunk(int cx, int cy, int cz, CChunk* chunk);
	void makeChunkPart(int cx, int cz, class CChunkPart* part);
	void generateWorld(std::tuple<int, int, int> playerPos, int distance, class CChunkManager* world);
	void consumeCache(CChunkManager* world);
	static std::vector<glm::ivec4> getTree(int i);
};

#endif