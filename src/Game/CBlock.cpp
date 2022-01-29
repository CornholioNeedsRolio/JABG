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
		m_blocks[BLOCK_AIR]->setTransparent(true);


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

		//LEAVES
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_LEAVES]->setSolid(true);
		m_blocks[BLOCK_LEAVES]->setVisible(true);
		for (uint8_t i = 0; i < 6; i++)
			m_blocks[BLOCK_LEAVES]->setFaceAtlasIndex(i, 5);

		//PLANKS
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_PLANKS]->setSolid(true);
		m_blocks[BLOCK_PLANKS]->setVisible(true);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 9);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 9);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 8);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 10);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 7);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 7);

		//BRICKS
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_BRICKS]->setSolid(true);
		m_blocks[BLOCK_BRICKS]->setVisible(true);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 11);

		//BROKENSTONE
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_BROKENSTONE]->setSolid(true);
		m_blocks[BLOCK_BROKENSTONE]->setVisible(true);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 12);

		//STONEBRICK
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_STONEBRICK]->setSolid(true);
		m_blocks[BLOCK_STONEBRICK]->setVisible(true);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 13);

		//STONEBRICK
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_GLASS]->setSolid(true);
		m_blocks[BLOCK_GLASS]->setVisible(true);
		m_blocks[BLOCK_GLASS]->setTransparent(true);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 14);

		//SAND
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_SAND]->setSolid(true);
		m_blocks[BLOCK_SAND]->setVisible(true);
		m_blocks[BLOCK_SAND]->setTransparent(true);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 15);

		//OLDSTONE
		m_blocks.push_back(std::make_unique<CBlock>());
		m_blocks[BLOCK_OLDSTONE]->setSolid(true);
		m_blocks[BLOCK_OLDSTONE]->setVisible(true);
		m_blocks[BLOCK_OLDSTONE]->setTransparent(true);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 16);
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
