#ifndef CBLOCK_HEADER
#define CBLOCK_HEADER
#include <vector>
#include "../CMesh.h"
#include "GameImportantStuff.h"
#include <bitset>


class CBlock
{
	static std::vector<SVertex> DefaultBlock;
protected:
	std::array<unsigned short, 6> m_faceatlasindex;
	//FLAGS
	enum
	{
		IS_SOLID,
		IS_VISIBLE,
		IS_TRANSPARENT,
		IS_TARGETABLE,
	};
	std::bitset<4> m_flags;
	//uint8_t m_solid;
	//uint8_t m_visible;
	ColliderType m_type = BLOCKCOLLIDER_FULL;
	const std::vector<SVertex>& GetDefaultBlock() const;
public:
	enum block_faces : uint8_t
	{
		BLOCK_FRONT = 0,
		BLOCK_BACK,
		BLOCK_RIGHT,
		BLOCK_LEFT,
		BLOCK_TOP,
		BLOCK_BOTTOM
	};
	bool isVisible() const { return m_flags[IS_VISIBLE]; };
	bool isSolid() const { return m_flags[IS_SOLID]; };
	bool isTransparent() const { return m_flags[IS_TRANSPARENT]; };
	bool isTargetable() const { return m_flags[IS_TARGETABLE]; };

	const ColliderType& getCollision();

	void setVisible(bool visible) { m_flags[IS_VISIBLE] = visible; };
	void setSolid(bool solid) { m_flags[IS_SOLID] = solid; };
	void setTransparent(bool transparent) { m_flags[IS_TRANSPARENT] = transparent; };
	void setTargetable(bool targetable) { m_flags[IS_TARGETABLE] = targetable; };
	void setCollisionType(ColliderType type);

	int getFaceAtlasIndex(uint8_t face) { return m_faceatlasindex[face]; };
	void setFaceAtlasIndex(uint8_t face, int index) { m_faceatlasindex[face] = index; };
	virtual std::vector<SVertex> getBlockMeshVertices(CBlock* neighbors[6], std::shared_ptr<class CTextureAtlas> texture, glm::ivec3 position = glm::ivec3(0));
};

enum
{
	BLOCK_AIR,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_LOG,
	BLOCK_LEAVES,
	BLOCK_PLANKS,
	BLOCK_BRICKS,
	BLOCK_BROKENSTONE,
	BLOCK_STONEBRICK,
	BLOCK_GLASS,
	BLOCK_SAND,
	BLOCK_OLDSTONE,
	BLOCK_PEBBLE,
	BLOCK_WATER,
	BLOCK_TOTAL
};

namespace BLOCK_DATABASE
{
	class Blocks
	{
		std::vector<std::unique_ptr<CBlock>> m_blocks;
	public:

		void Init();
		CBlock* getBlock(int id);
	};
	extern Blocks Database;
	void Init();
	CBlock* getBlock(int id);
};

#endif
