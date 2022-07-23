#include "SpecialBlocks/CPable.h"
#include "../CTextureAtlas.h"
#include "CBlock.h"
#include "SpecialBlocks/CDirt.h"
#include "SpecialBlocks/CGrass.h"
#include <iostream>


const ColliderType& CBlock::getCollision()
{
	return m_type;
}

void CBlock::setCollisionType(ColliderType type)
{
	m_type = type;
}

const std::vector<SVertex>& CBlock::GetDefaultBlock() const
{
	return DefaultBlock;
}

constexpr int visibleSides[] = {22, 4, 14, 12, 16, 10};
CBlock::CBlock()
{
	m_neededNeighbors.reset();
	for(int i = 0; i < 6; ++i)
		m_neededNeighbors[visibleSides[i]] = 1;
}

std::vector<SVertex> CBlock::getBlockMeshVertices(CBlock* neighbors[27], std::shared_ptr<class CTextureAtlas> texture, glm::ivec3 position)
{
	const std::vector<SVertex>& _temp = GetDefaultBlock();

	std::vector<SVertex> output = {};
	for(int i = 0; i < 6; i++)
	{
		if(neighbors[visibleSides[i]]) if(!neighbors[visibleSides[i]]->isTransparent()) continue;
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



class CBlock* BlockAir = nullptr;
class CGrass* BlockGrass = nullptr;
class CDirt* BlockDirt = nullptr;
class CBlock* BlockStone = nullptr;
class CBlock* BlockLog = nullptr;
class CBlock* BlockPlanks = nullptr;
class CBlock* BlockBricks = nullptr;
class CBlock* BlockBrokenStone = nullptr;
class CBlock* BlockStoneBrick= nullptr;
class CBlock* BlockGlass = nullptr;
class CBlock* BlockSand = nullptr;
class CBlock* BlockOldstone = nullptr;
class CPable* BlockPebble = nullptr;
class CBlock* BlockWater = nullptr;
class CBlock* BlockLeaves = nullptr;

std::vector<SVertex> CBlock::DefaultBlock =  {

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









namespace BLOCK_DATABASE
{
	void Blocks::Init()
	{
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockAir = m_blocks[BLOCK_AIR].get();
		m_blocks[BLOCK_AIR]->setSolid(false);
		m_blocks[BLOCK_AIR]->setTargetable(false);
		m_blocks[BLOCK_AIR]->setVisible(false);
		m_blocks[BLOCK_AIR]->setCollisionType(BLOCKCOLLIDER_NONE);
		m_blocks[BLOCK_AIR]->setTransparent(true);

		//GRASS
		m_blocks.push_back(std::make_unique<CGrass>());
		BlockGrass = (CGrass*)m_blocks[BLOCK_GRASS].get();
		m_blocks[BLOCK_GRASS]->setSolid(true);
		m_blocks[BLOCK_GRASS]->setVisible(true);
		m_blocks[BLOCK_GRASS]->setTargetable(true);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 2);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 0);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 1);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 1);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 1);
		m_blocks[BLOCK_GRASS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 1);
		
		//DIRT
		m_blocks.push_back(std::make_unique<CDirt>());
		BlockDirt = (CDirt*)m_blocks[BLOCK_DIRT].get();
		BlockDirt->setSolid(true);
		BlockDirt->setVisible(true);
		BlockDirt->setTargetable(true);
		for(uint8_t i = 0; i < 6; ++i) BlockDirt->setFaceAtlasIndex(i, 2);
		for(uint8_t i = 0; i < 4; ++i) BlockDirt->SetGrassFace(1, i);

		//STONE
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockStone = m_blocks[BLOCK_STONE].get();
		m_blocks[BLOCK_STONE]->setSolid(true);
		m_blocks[BLOCK_STONE]->setVisible(true);
		m_blocks[BLOCK_STONE]->setTargetable(true);
		for (uint8_t i = 0; i < 6; i++)
			m_blocks[BLOCK_STONE]->setFaceAtlasIndex(i, 3);

		//BLOCK_LOG
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockLog = m_blocks[BLOCK_LOG].get();
		m_blocks[BLOCK_LOG]->setSolid(true);
		m_blocks[BLOCK_LOG]->setVisible(true);
		m_blocks[BLOCK_LOG]->setTargetable(true);
		for (uint8_t i = 0; i < 4; i++)
			m_blocks[BLOCK_LOG]->setFaceAtlasIndex(i, 4);
		m_blocks[BLOCK_LOG]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 6);
		m_blocks[BLOCK_LOG]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 6);

		//LEAVES
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockLeaves = m_blocks[BLOCK_LOG].get();
		m_blocks[BLOCK_LEAVES]->setSolid(true);
		m_blocks[BLOCK_LEAVES]->setVisible(true);
		m_blocks[BLOCK_LEAVES]->setTargetable(true);
		for (uint8_t i = 0; i < 6; i++)
			m_blocks[BLOCK_LEAVES]->setFaceAtlasIndex(i, 5);

		//PLANKS
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockPlanks = m_blocks[BLOCK_PLANKS].get();
		m_blocks[BLOCK_PLANKS]->setSolid(true);
		m_blocks[BLOCK_PLANKS]->setVisible(true);
		m_blocks[BLOCK_PLANKS]->setTargetable(true);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 9);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 9);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 8);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 10);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 7);
		m_blocks[BLOCK_PLANKS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 7);

		//BRICKS
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockBricks = m_blocks[BLOCK_BRICKS].get();
		m_blocks[BLOCK_BRICKS]->setSolid(true);
		m_blocks[BLOCK_BRICKS]->setVisible(true);
		m_blocks[BLOCK_BRICKS]->setTargetable(true);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 11);
		m_blocks[BLOCK_BRICKS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 11);

		//BROKENSTONE
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockBrokenStone = m_blocks[BLOCK_BROKENSTONE].get();
		m_blocks[BLOCK_BROKENSTONE]->setSolid(true);
		m_blocks[BLOCK_BROKENSTONE]->setVisible(true);
		m_blocks[BLOCK_BROKENSTONE]->setTargetable(true);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 12);
		m_blocks[BLOCK_BROKENSTONE]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 12);

		//STONEBRICK
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockStoneBrick = m_blocks[BLOCK_STONEBRICK].get();
		m_blocks[BLOCK_STONEBRICK]->setSolid(true);
		m_blocks[BLOCK_STONEBRICK]->setVisible(true);
		m_blocks[BLOCK_STONEBRICK]->setTargetable(true);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 13);
		m_blocks[BLOCK_STONEBRICK]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 13);

		//STONEBRICK
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockGlass = m_blocks[BLOCK_GLASS].get();
		m_blocks[BLOCK_GLASS]->setSolid(true);
		m_blocks[BLOCK_GLASS]->setVisible(true);
		m_blocks[BLOCK_GLASS]->setTransparent(true);
		m_blocks[BLOCK_GLASS]->setTargetable(true);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 14);
		m_blocks[BLOCK_GLASS]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 14);

		//SAND
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockSand = m_blocks[BLOCK_SAND].get();
		m_blocks[BLOCK_SAND]->setSolid(true);
		m_blocks[BLOCK_SAND]->setVisible(true);
		m_blocks[BLOCK_SAND]->setTargetable(true);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 15);
		m_blocks[BLOCK_SAND]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 15);

		//OLDSTONE
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockOldstone = m_blocks[BLOCK_OLDSTONE].get();
		m_blocks[BLOCK_OLDSTONE]->setSolid(true);
		m_blocks[BLOCK_OLDSTONE]->setVisible(true);
		m_blocks[BLOCK_OLDSTONE]->setTargetable(true);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 16);
		m_blocks[BLOCK_OLDSTONE]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 16);

		//PEBLE
		m_blocks.push_back(std::make_unique<CPable>());
		BlockPebble = (CPable*)m_blocks[BLOCK_PEBBLE].get();
		//WATER
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockWater = m_blocks[BLOCK_WATER].get();
		m_blocks[BLOCK_WATER]->setSolid(false);
		m_blocks[BLOCK_WATER]->setVisible(true);
		m_blocks[BLOCK_WATER]->setTargetable(false);
		m_blocks[BLOCK_WATER]->setFaceAtlasIndex(CBlock::BLOCK_TOP, 17);
		m_blocks[BLOCK_WATER]->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 17);
		m_blocks[BLOCK_WATER]->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 17);
		m_blocks[BLOCK_WATER]->setFaceAtlasIndex(CBlock::BLOCK_BACK, 17);
		m_blocks[BLOCK_WATER]->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 17);
		m_blocks[BLOCK_WATER]->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 17);
		m_blocks[BLOCK_WATER]->setTransparent(false);
	}

	CBlock* Blocks::getBlock(int id)
	{
		if (BLOCK_TOTAL > id)
			return m_blocks[id].get();
		std::cout << "[BLOCK]Outside of range " << id;
		//throw;
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