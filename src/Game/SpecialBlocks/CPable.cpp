#include "CPable.h"
#include "Engine/CTextureAtlas.h"

CPable::CPable()
{
    m_flags[IS_VISIBLE] = true;
    m_flags[IS_TARGETABLE] = true;
    m_flags[IS_TRANSPARENT] = true;
    m_flags[IS_SOLID] = false;
    for(int i = 0; i < m_faceatlasindex.size(); ++i)
        m_faceatlasindex[i] = 3;
}

std::vector<SVertex> CPable::getBlockMeshVertices(CBlock* neighbors[27], std::shared_ptr<CTextureAtlas> texture, glm::ivec3 position)
{
	const std::vector<SVertex>& _temp = GetDefaultBlock();

	std::vector<SVertex> output = {};
    const float offset = 0.5f-m_scale*0.5f;
    srand(position.x ^ position.z);
    const float offsetx = offset + ((rand()&63)/256.f)*glm::sign((rand()&1) - 2);
    const float offsety = std::max(-0.2f, -offset*((rand()&31)/32.f));
    const float offsetz = offset + ((rand()&63)/256.f)*glm::sign((rand()&1) - 2);
	for(int i = 0; i < 6; i++)
	{
		std::vector<SVertex> face = {};
		face.insert(face.end(), _temp.begin()+i*6, _temp.begin()+i*6+6);
		for (int i = 0; i < face.size(); i++)
		{
			face[i].position.x = offsetx + face[i].position.x * m_scale + position.x;
			face[i].position.y = face[i].position.y * m_scale + position.y+offsety;
			face[i].position.z = offsetz + face[i].position.z * m_scale + position.z;
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