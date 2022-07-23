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

void CChunkMeshComponent::BuildMeshData(std::array<CChunk*, 27> neighbors, std::shared_ptr<CTextureAtlas> atlas)
{
    struct s{
        CChunkMeshComponent* m_parent = nullptr;
        s(CChunkMeshComponent* parent) : m_parent(parent) { parent->m_flags |= CChunkMeshComponent::THREAD_RUNNING; };
        ~s() { m_parent->m_flags = (m_parent->m_flags & ~CChunkMeshComponent::THREAD_RUNNING) | CChunkMeshComponent::THREAD_DONE; };
    }ss(this);

    auto getBlock = [&](int x, int y, int z)->SBlockInfo
	{
		int chunk_x = 1, chunk_y = 1, chunk_z = 1;
		int local_x = x, local_y = y, local_z = z;
		if(z <= -1) {
			--chunk_z; 	local_z = CHUNK_SIZE-1;
		}else if(z >= CHUNK_SIZE) {
			++chunk_z; local_z = 0;
		}

		if(x <= -1) {
			--chunk_x;  local_x = CHUNK_SIZE-1;
		} else if(x >= CHUNK_SIZE) {
			++chunk_x; local_x = 0;
		}

		if(y <= -1) {
			--chunk_y;  local_y = CHUNK_SIZE-1;
		} else if(y >= CHUNK_SIZE) {
			++chunk_y; local_y = 0;
		}

		int index = chunk_x + 3*(chunk_y+chunk_z*3);
		if(index == 13) 
			return m_parent->getVoxelComponent().getBlock(x, y, z);
		if(neighbors[index]) 
			return neighbors[index]->getVoxelComponent().getBlock(local_x, local_y, local_z);
		
		//if (x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE)
		//	return m_parent->getVoxelComponent().getBlock(x, y, z);
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

                CBlock* temp[27] = {};
				for(int xx = -1; xx <= 1; ++xx)
				{
					for(int yy = -1; yy <= 1; ++yy)
					{
						for(int zz = -1; zz <= 1; ++zz)
						{
							int index = (xx+1)+3*((yy+1) + ((zz+1)*3));

							if(self->IsNeightborNeeded(index)) 
							{
								if(getBlock(xx+x, yy+y, zz+z).getBlock() == nullptr) throw;
								temp[index] = getBlock(xx+x, yy+y, zz+z).getBlock();
							}
							else 
								temp[index] = nullptr;
						}
					}
				}
                /*temp[CBlock::BLOCK_BACK]   = getBlock(x, y, z-1).getBlock();
				temp[CBlock::BLOCK_FRONT]  = getBlock(x, y, z+1).getBlock();
				temp[CBlock::BLOCK_RIGHT]  = getBlock(x+1, y, z).getBlock();
				temp[CBlock::BLOCK_LEFT]   = getBlock(x-1, y, z).getBlock();
				temp[CBlock::BLOCK_TOP]    = getBlock(x, y+1, z).getBlock();
				temp[CBlock::BLOCK_BOTTOM] = getBlock(x, y-1, z).getBlock();*/

				auto blockmeshinfo = self->getBlockMeshVertices(temp, atlas, {x, y, z});

				m_meshinfo.insert(m_meshinfo.end(), blockmeshinfo.begin(), blockmeshinfo.end());
            }
        }
    }
}