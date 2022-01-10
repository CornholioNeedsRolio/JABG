#include "CBlock.h"
#include "../CTextureAtlas.h"
#include <iostream>


const ColliderType& CBlock::getCollision()
{
	return m_type;
}

void CBlock::setCollisionType(ColliderType type)
{
	m_type = type;
}

std::vector<SVertex> CBlock::getBlockMeshVertices(bool faces[6], std::shared_ptr<class CTextureAtlas> texture, glm::ivec3 position)
{
	std::vector<SVertex> _temp = {

		//FRONT
		{{1, 1, 1},{0, 0}, {0, 0, 1}},
		{{0, 1, 1},{1, 0}, {0, 0, 1}},
		{{0, 0, 1},{1, 1}, {0, 0, 1}},

		{{0, 0, 1},{1, 1}, {0, 0, 1}},
		{{1, 0, 1},{0, 1}, {0, 0, 1}},
		{{1, 1, 1},{0, 0}, {0, 0, 1}},

		//BACK
		{{0, 1, 0},{0, 0}, {0,0,-1}},
		{{1, 1, 0},{1, 0}, {0,0,-1}},
		{{1, 0, 0},{1, 1}, {0,0,-1}},

		{{1, 0, 0},{1, 1}, {0,0,-1}},
		{{0, 0, 0},{0, 1}, {0,0,-1}},
		{{0, 1, 0},{0, 0}, {0,0,-1}},

		//RIGHT
		{{1, 1, 0},{0, 0}, {1, 0, 0}},
		{{1, 1, 1},{1, 0}, {1, 0, 0}},
		{{1, 0, 1},{1, 1}, {1, 0, 0}},

		{{1, 0, 1},{1, 1}, {1, 0, 0}},
		{{1, 0, 0},{0, 1}, {1, 0, 0}},
		{{1, 1,  0},{0,0}, {1, 0, 0}},

		//LEFT
		{{0, 1, 1},{0, 0}, {-1, 0, 0}},
		{{0, 1, 0},{1, 0}, {-1, 0, 0}},
		{{0, 0, 0},{1, 1}, {-1, 0, 0}},

		{{0, 0, 0},{1, 1}, {-1, 0, 0}},
		{{0, 0, 1},{0, 1}, {-1, 0, 0}},
		{{0, 1, 1},{0, 0}, {-1, 0, 0}},

		//TOP
		{{1, 1, 0},{0, 0}, {0, 1, 0}},
		{{0, 1, 0},{1, 0}, {0, 1, 0}},
		{{0, 1, 1},{1, 1}, {0, 1, 0}},

		{{0, 1, 1},{1, 1}, {0, 1, 0}},
		{{1, 1, 1},{0, 1}, {0, 1, 0}},
		{{1, 1, 0},{0, 0}, {0, 1, 0}},

		//BOTTOM
		{{0, 0, 0},{0, 0}, {0, -1, 0}},
		{{1, 0, 0},{1, 0}, {0, -1, 0}},
		{{1, 0, 1},{1, 1}, {0, -1, 0}},

		{{1, 0, 1},{1, 1}, {0, -1, 0}},
		{{0, 0, 1},{0, 1}, {0, -1, 0}},
		{{0, 0, 0},{0, 0}, {0, -1, 0}}

	};

	std::vector<SVertex> output = {};
	for(int i = 0; i < 6; i++)
	{
		if(!faces[i]) continue;
		std::vector<SVertex> face = {};
		face.insert(face.end(), _temp.begin()+i*6, _temp.begin()+i*6+6);
		for (int i = 0; i < face.size(); i++)
		{
			face[i].position.x += position.x;
			face[i].position.y += position.y;
			face[i].position.z += position.z;
		}

		auto tex = texture->GetFromIndex(getFaceAtlasIndex((block_faces)i));
		for(short i = 0; i < 6; i++)
		{
			face[i].texture.u = tex[i*2];
			face[i].texture.v = tex[i*2+1];
		}
		
		output.insert(output.end(), face.begin(), face.end());
	}

	return output;
}


namespace BLOCK_DATABASE
{
	void Blocks::Init()
	{
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_AIR]->setSolid(false);
		m_blocks[BLOCK_AIR]->setVisible(false);
		m_blocks[BLOCK_AIR]->setCollisionType(BLOCKCOLLIDER_NONE);


		//GRASS
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_GRASS]->setSolid(true);
		m_blocks[BLOCK_GRASS]->setVisible(true);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 2);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 0);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 1);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 1);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 1);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 1);
		
		//DIRT
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_DIRT]->setSolid(true);
		m_blocks[BLOCK_DIRT]->setVisible(true);
		for(uint8_t i = 0; i < 6; i++)
			m_blocks[BLOCK_DIRT]->setFaceAtlasIndex(i, 2);

		//STONE
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_STONE]->setSolid(true);
		m_blocks[BLOCK_STONE]->setVisible(true);
		for (uint8_t i = 0; i < 6; i++)
			m_blocks[BLOCK_STONE]->setFaceAtlasIndex(i, 3);

		//BLOCK_LOG
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_LOG]->setSolid(true);
		m_blocks[BLOCK_LOG]->setVisible(true);
		for (uint8_t i = 0; i < 4; i++)
			m_blocks[BLOCK_LOG]->setFaceAtlasIndex(i, 4);
		m_blocks[BLOCK_LOG]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 6);
		m_blocks[BLOCK_LOG]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 6);

		//STONE
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_LEAVES]->setSolid(true);
		m_blocks[BLOCK_LEAVES]->setVisible(true);
		for (uint8_t i = 0; i < 6; i++)
			m_blocks[BLOCK_LEAVES]->setFaceAtlasIndex(i, 5);
	}

	CBlock* Blocks::getBlock(int id)
	{
		if (BLOCK_TOTAL > id)
			return m_blocks[id].get();
		std::cout << "[BLOCK]Outside of range";
		return nullptr;
	}
	Blocks Database;

	void Init()
	{
		Database.Init();
	}

	CBlock* getBlock(int id)
	{
		return Database.getBlock(id);
	}
}
