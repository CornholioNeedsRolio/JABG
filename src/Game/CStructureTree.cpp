#include "CStructureTree.h"
#include "CChunk.h"
#include <iostream>

bool CStructureTree::shouldBeHere(int x, int z)
{
	srand((x*0x23423134) ^ (z*0x5325312));
	return rand()%200<1;
}

void CStructureTree::getInfo(int x, int y, int z, StructureInfo& info)
{
	std::vector <std::tuple<int, int, std::pair<int, int>>> treeData = {};
	for (int i = 0; i <= 5; i++)
		treeData.push_back(std::make_tuple(x, y + i, std::make_pair(z, BLOCK_LOG)));

	for (int i = 3; i <= 6; i++)
		for (int cx = -3; cx <= 3; cx++)
			for (int cz = -3; cz <= 3; cz++)
			{
				srand((i+x+y+(cx*0x23423134)) ^ (cz*0x5325312));
				bool shouldBeThere = true;
				if(cx != 0 || cz != 0)
				{
					if(std::abs(cx) == 3 || std::abs(cz) == 3 || std::abs(i-3) == 3)
						if(rand() % 2 == 0)
							shouldBeThere = false;
				}
				else if(i > 5)
					shouldBeThere = true;
				else 
					shouldBeThere = false;
				if(shouldBeThere)
					treeData.push_back(std::make_tuple(x + cx, y + i, std::make_pair(z + cz, BLOCK_LEAVES)));
			}

	for (int i = 0; i < treeData.size(); i++)
	{
		int bx = std::get<0>(treeData[i]);
		int by = std::get<1>(treeData[i]);
		int bz = std::get<2>(treeData[i]).first;
		int block = std::get<2>(treeData[i]).second;

		int cx = std::floor(float(bx) / float(CHUNK_SIZE));
		int cy = std::floor(float(by) / float(CHUNK_SIZE));
		int cz = std::floor(float(bz) / float(CHUNK_SIZE));

		bx %= CHUNK_SIZE;
		if (bx < 0) bx += CHUNK_SIZE;
		by %= CHUNK_SIZE;
		if (by < 0) by += CHUNK_SIZE;
		bz %= CHUNK_SIZE;
		if (bz < 0) bz += CHUNK_SIZE;

		bool found = false;
		uint32_t index = 0;
		for (uint32_t j = 0; j < info.size(); j++)
			if (info[j].first == std::make_tuple(cx, cy, cz))
			{
				found = true;
				index = j;
				break;
			}
		if (!found) 
		{
			index = info.size();
			info.push_back(std::make_pair(std::make_tuple(cx, cy, cz), std::vector<std::pair<std::tuple<int, int, int>, int>>()));
		}
	
		info[index].second.push_back(std::make_pair(std::make_tuple(bx, by, bz), block));
		//info[index].second.push_back(std::make_pair(std::make_tuple(bx % CHUNK_SIZE + (bx < 0) * CHUNK_SIZE, by % CHUNK_SIZE + (by < 0) * CHUNK_SIZE, bz % CHUNK_SIZE + (bz < 0) * CHUNK_SIZE), block));
	}
}
