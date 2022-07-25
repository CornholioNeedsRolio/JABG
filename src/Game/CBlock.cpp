#include "SpecialBlocks/CPable.h"
#include "Engine/CTextureAtlas.h"
#include "CBlock.h"
#include "SpecialBlocks/CDirt.h"
#include "SpecialBlocks/CGrass.h"
#include "SpecialBlocks/CConnectedTexBlock.h"
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

uint8_t CBlock::getFaceAt(CBlock* neighbors[27], uint8_t face) const
{
	return getFaceAtlasIndex(face);
}

bool CBlock::IsFaceVisible(CBlock* facingBlock) const
{
	if(!facingBlock) return false;
	return !facingBlock->isTransparent() && blockMeshType == facingBlock->blockMeshType;
}

void CBlock::setHeight(float height)
{
	m_height = height;
}

std::vector<SVertex> CBlock::getBlockMeshVertices(CBlock* neighbors[27], std::shared_ptr<class CTextureAtlas> texture, glm::ivec3 position)
{
	const std::vector<SVertex>& _temp = GetDefaultBlock();

	std::vector<SVertex> output = {};
	for(int i = 0; i < 6; i++)
	{
		if(IsFaceVisible(neighbors[visibleSides[i]])) continue;
		std::vector<SVertex> face = {};
		float yoffset = 0;
		if(i == 4) yoffset = 1.f - m_height;
		face.insert(face.end(), _temp.begin()+i*6, _temp.begin()+i*6+6);
		for (int i = 0; i < face.size(); i++)
		{
			face[i].position.x += position.x;
			face[i].position.y += position.y - yoffset;
			face[i].position.z += position.z;
		}

		auto tex = texture->GetFromIndex(getFaceAt(neighbors, i));
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
class CConnectedTexBlock* BlockGlass = nullptr;
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
		BlockAir->setSolid(false);
		BlockAir->setTargetable(false);
		BlockAir->setVisible(false);
		BlockAir->setCollisionType(BLOCKCOLLIDER_NONE);
		BlockAir->setTransparent(true);

		//GRASS
		m_blocks.push_back(std::make_unique<CGrass>());
		BlockGrass = (CGrass*)m_blocks[BLOCK_GRASS].get();
		BlockGrass->setSolid(true);
		BlockGrass->setVisible(true);
		BlockGrass->setTargetable(true);
		BlockGrass->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 2);
		BlockGrass->setFaceAtlasIndex(CBlock::BLOCK_TOP, 0);
		BlockGrass->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 1);
		BlockGrass->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 1);
		BlockGrass->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 1);
		BlockGrass->setFaceAtlasIndex(CBlock::BLOCK_BACK, 1);
		
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
		BlockStone->setSolid(true);
		BlockStone->setVisible(true);
		BlockStone->setTargetable(true);
		for (uint8_t i = 0; i < 6; i++)
			BlockStone->setFaceAtlasIndex(i, 3);

		//BLOCK_LOG
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockLog = m_blocks[BLOCK_LOG].get();
		BlockLog->setSolid(true);
		BlockLog->setVisible(true);
		BlockLog->setTargetable(true);
		for (uint8_t i = 0; i < 4; i++)
			BlockLog->setFaceAtlasIndex(i, 4);
		BlockLog->setFaceAtlasIndex(CBlock::BLOCK_TOP, 6);
		BlockLog->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 6);

		//LEAVES
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockLeaves = m_blocks[BLOCK_LEAVES].get();
		BlockLeaves->setSolid(true);
		BlockLeaves->setVisible(true);
		BlockLeaves->setTargetable(true);
		for (uint8_t i = 0; i < 6; i++)
			BlockLeaves->setFaceAtlasIndex(i, 5);

		//PLANKS
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockPlanks = m_blocks[BLOCK_PLANKS].get();
		BlockPlanks->setSolid(true);
		BlockPlanks->setVisible(true);
		BlockPlanks->setTargetable(true);
		BlockPlanks->setFaceAtlasIndex(CBlock::BLOCK_TOP, 9);
		BlockPlanks->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 9);
		BlockPlanks->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 8);
		BlockPlanks->setFaceAtlasIndex(CBlock::BLOCK_BACK, 10);
		BlockPlanks->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 7);
		BlockPlanks->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 7);

		//BRICKS
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockBricks = m_blocks[BLOCK_BRICKS].get();
		BlockBricks->setSolid(true);
		BlockBricks->setVisible(true);
		BlockBricks->setTargetable(true);
		BlockBricks->setFaceAtlasIndex(CBlock::BLOCK_TOP, 11);
		BlockBricks->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 11);
		BlockBricks->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 11);
		BlockBricks->setFaceAtlasIndex(CBlock::BLOCK_BACK, 11);
		BlockBricks->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 11);
		BlockBricks->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 11);

		//BROKENSTONE
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockBrokenStone = m_blocks[BLOCK_BROKENSTONE].get();
		BlockBrokenStone->setSolid(true);
		BlockBrokenStone->setVisible(true);
		BlockBrokenStone->setTargetable(true);
		BlockBrokenStone->setFaceAtlasIndex(CBlock::BLOCK_TOP, 12);
		BlockBrokenStone->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 12);
		BlockBrokenStone->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 12);
		BlockBrokenStone->setFaceAtlasIndex(CBlock::BLOCK_BACK, 12);
		BlockBrokenStone->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 12);
		BlockBrokenStone->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 12);

		//STONEBRICK
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockStoneBrick = m_blocks[BLOCK_STONEBRICK].get();
		BlockStoneBrick->setSolid(true);
		BlockStoneBrick->setVisible(true);
		BlockStoneBrick->setTargetable(true);
		BlockStoneBrick->setFaceAtlasIndex(CBlock::BLOCK_TOP, 13);
		BlockStoneBrick->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 13);
		BlockStoneBrick->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 13);
		BlockStoneBrick->setFaceAtlasIndex(CBlock::BLOCK_BACK, 13);
		BlockStoneBrick->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 13);
		BlockStoneBrick->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 13);

		//GLASS
		m_blocks.push_back(std::make_unique<CConnectedTexBlock>());
		BlockGlass = (CConnectedTexBlock*)m_blocks[BLOCK_GLASS].get();
		BlockGlass->setSolid(true);
		BlockGlass->setVisible(true);
		BlockGlass->setTransparent(false);
		BlockGlass->setTargetable(true);
        BlockGlass->setMeshType(BLOCKMESH_GLASS);
        constexpr int glassAtlasIndexes[16] = {35, 82, 67, 49, 34, 50, 33, 80, 64, 48, 65, 83, 32, 81, 51, 66};
        for(int i = 0; i < 16; ++i)
           BlockGlass->setFaceAt(i, glassAtlasIndexes[i]);
		/*BlockGlass->setFaceAtlasIndex(CBlock::BLOCK_TOP, 14);
		BlockGlass->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 14);
		BlockGlass->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 14);
		BlockGlass->setFaceAtlasIndex(CBlock::BLOCK_BACK, 14);
		BlockGlass->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 14);
		BlockGlass->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 14);*/

		//SAND
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockSand = m_blocks[BLOCK_SAND].get();
		BlockSand->setSolid(true);
		BlockSand->setVisible(true);
		BlockSand->setTargetable(true);
		BlockSand->setFaceAtlasIndex(CBlock::BLOCK_TOP, 15);
		BlockSand->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 15);
		BlockSand->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 15);
		BlockSand->setFaceAtlasIndex(CBlock::BLOCK_BACK, 15);
		BlockSand->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 15);
		BlockSand->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 15);

		//OLDSTONE
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockOldstone = m_blocks[BLOCK_OLDSTONE].get();
		BlockOldstone->setSolid(true);
		BlockOldstone->setVisible(true);
		BlockOldstone->setTargetable(true);
		BlockOldstone->setFaceAtlasIndex(CBlock::BLOCK_TOP, 16);
		BlockOldstone->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 16);
		BlockOldstone->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 16);
		BlockOldstone->setFaceAtlasIndex(CBlock::BLOCK_BACK, 16);
		BlockOldstone->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 16);
		BlockOldstone->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 16);

		//PEBLE
		m_blocks.push_back(std::make_unique<CPable>());
		BlockPebble = (CPable*)m_blocks[BLOCK_PEBBLE].get();
		//WATER
		m_blocks.push_back(std::make_unique<CBlock>());
		BlockWater = m_blocks[BLOCK_WATER].get();
		BlockWater->setSolid(false);
		BlockWater->setVisible(true);
		BlockWater->setTargetable(true);
		BlockWater->setMeshType(BLOCKMESH_WATER);
		BlockWater->setFaceAtlasIndex(CBlock::BLOCK_TOP, 17);
		BlockWater->setFaceAtlasIndex(CBlock::BLOCK_BOTTOM, 17);
		BlockWater->setFaceAtlasIndex(CBlock::BLOCK_FRONT, 17);
		BlockWater->setFaceAtlasIndex(CBlock::BLOCK_BACK, 17);
		BlockWater->setFaceAtlasIndex(CBlock::BLOCK_RIGHT, 17);
		BlockWater->setFaceAtlasIndex(CBlock::BLOCK_LEFT, 17);
		BlockWater->setTransparent(false);
		BlockWater->setHeight(1.f-0.0625);
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