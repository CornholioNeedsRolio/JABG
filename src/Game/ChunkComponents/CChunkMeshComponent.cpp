#include "../../BulkRenderer/CBulkRenderer.h"
#include "CChunkVoxelComponent.h"
#include "../../CTextureAtlas.h"
#include "CChunkMeshComponent.h"
#include "../CChunk.h"

CChunkMeshComponent::CChunkMeshComponent(CChunk* parent) : m_parent(parent)
{

}

CChunkMeshComponent::~CChunkMeshComponent()
{
	while(m_flags & THREAD_RUNNING);
}

void CChunkMeshComponent::setTextureAndShader(std::shared_ptr<CTextureAtlas> texture, std::shared_ptr<CShader> shader)
{
	m_mesh.SetTexture(texture);
	m_mesh.SetShader(shader);
}

void CChunkMeshComponent::Draw(CBulkRenderer* renderer)
{
    if (m_flags & THREAD_DONE && (m_flags & THREAD_RUNNING) == 0)
	{
		m_mesh.Clear();

		if (!m_meshinfo.empty())
		{
			m_mesh.Init(m_meshinfo);
			m_meshinfo.clear();
		}
		m_flags &= ~THREAD_DONE;
	}
	
	if (!m_mesh.isInit())
		return;
	CCamera* camera = renderer->getActiveCamera();
	if(!m_parent->fustumCulling(camera)) 
		return;
	m_mesh.SetPosition(m_parent->getChunkPosition() * CHUNK_SIZE);
	m_mesh.BulkDraw(renderer);
}

void CChunkMeshComponent::makeDirty()
{
    ++m_currentDirty;
}

bool CChunkMeshComponent::isDirty() const
{
    return m_currentDirty >= m_lastDirty;
}

void CChunkMeshComponent::BuildMeshData(std::array<CChunk*, 6> neighbors, std::shared_ptr<CTextureAtlas> atlas)
{
    struct s{
        CChunkMeshComponent* m_parent = nullptr;
        s(CChunkMeshComponent* parent) : m_parent(parent) { parent->m_flags |= CChunkMeshComponent::THREAD_RUNNING; };
        ~s() { m_parent->m_flags = (m_parent->m_flags & ~CChunkMeshComponent::THREAD_RUNNING) | CChunkMeshComponent::THREAD_DONE; };
    }ss(this);

    auto getBlock = [&](int x, int y, int z)->SBlockInfo
	{
		if (z == -1 && neighbors[CHUNKFACE_BACK])
			return neighbors[CHUNKFACE_BACK]->getVoxelComponent().getBlock(x, y, CHUNK_SIZE-1);
		if (z == CHUNK_SIZE && neighbors[CHUNKFACE_FRONT])
			return neighbors[CHUNKFACE_FRONT]->getVoxelComponent().getBlock(x, y, 0);

		if (x == -1 && neighbors[CHUNKFACE_LEFT])
			return neighbors[CHUNKFACE_LEFT]->getVoxelComponent().getBlock(CHUNK_SIZE - 1, y, z);
		if (x == CHUNK_SIZE && neighbors[CHUNKFACE_RIGHT])
			return neighbors[CHUNKFACE_RIGHT]->getVoxelComponent().getBlock(0, y, z);

		if (y == -1 && neighbors[CHUNKFACE_BOT])
			return neighbors[CHUNKFACE_BOT]->getVoxelComponent().getBlock(x, CHUNK_SIZE - 1, z);
		if (y == CHUNK_SIZE && neighbors[CHUNKFACE_TOP])
			return neighbors[CHUNKFACE_TOP]->getVoxelComponent().getBlock(x, 0, z);

		/*
			*TODO: make it fetch from neighbors, in a safe manner 
            WELL NOW IT'S DONE
		*/
		if (z == -1 && neighbors[CHUNKFACE_BACK])
			return neighbors[CHUNKFACE_BACK]->getVoxelComponent().getBlock(x ,y, z);
		
		if (x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE)
			return m_parent->getVoxelComponent().getBlock(x, y, z);
		return {0};
	};

    m_lastDirty = m_currentDirty + 1;
    for(char x = 0; x < CHUNK_SIZE; ++x)
    {
        for(char y = 0; y < CHUNK_SIZE; ++y)
        {
            for(char z = 0; z < CHUNK_SIZE; ++z)
            {
                if(isDirty())
                    return;

                CBlock* self = m_parent->getVoxelComponent().getBlock(x, y, z).getBlock();
                if(!self->isVisible())
                    continue;

                bool temp[6] = {};

                temp[CBlock::BLOCK_BACK]   = getBlock(x, y, z-1).getBlock()->isTransparent();
				temp[CBlock::BLOCK_FRONT]  = getBlock(x, y, z+1).getBlock()->isTransparent();
				temp[CBlock::BLOCK_RIGHT]  = getBlock(x+1, y, z).getBlock()->isTransparent();
				temp[CBlock::BLOCK_LEFT]   = getBlock(x-1, y, z).getBlock()->isTransparent();
				temp[CBlock::BLOCK_TOP]    = getBlock(x, y+1, z).getBlock()->isTransparent();
				temp[CBlock::BLOCK_BOTTOM] = getBlock(x, y-1, z).getBlock()->isTransparent();

				auto blockmeshinfo = self->getBlockMeshVertices(temp, atlas, {x, y, z});

				m_meshinfo.insert(m_meshinfo.end(), blockmeshinfo.begin(), blockmeshinfo.end());
            }
        }
    }
}