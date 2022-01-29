#include "CSkyBox.h"
#include <iostream>

CSkyBox::CSkyBox()
{

}

void CSkyBox::Init(std::shared_ptr<class CTexture> texture)
{

	std::vector<SVertex> _temp = {

		//FRONT
		{{1, 1, 1},{0, 0}},
		{{0, 0, 1},{1, 1}},
		{{0, 1, 1},{1, 0}},

		{{0, 0, 1},{1, 1}},
		{{1, 1, 1},{0, 0}},
		{{1, 0, 1},{0, 1}},


        //LEFT
		{{0, 1, 1},{0, 0}},
		{{0, 0, 0},{1, 1}},
		{{0, 1, 0},{1, 0}},

		{{0, 0, 0},{1, 1}},
		{{0, 1, 1},{0, 0}},
		{{0, 0, 1},{0, 1}},

		//BACK
		{{0, 1, 0},{0, 0}},
		{{1, 0, 0},{1, 1}},
		{{1, 1, 0},{1, 0}},

		{{1, 0, 0},{1, 1}},
		{{0, 1, 0},{0, 0}},
		{{0, 0, 0},{0, 1}},

        //RIGHT
		{{1, 1, 0},{0, 0}},
		{{1, 0, 1},{1, 1}},
		{{1, 1, 1},{1, 0}},

		{{1, 0, 1},{1, 1}},
		{{1, 1,  0},{0,0}},
		{{1, 0, 0},{0, 1}},


		//TOP
		{{1, 1, 0},{0, 0}},
		{{0, 1, 1},{1, 1}},
		{{0, 1, 0},{1, 0}},

		{{0, 1, 1},{1, 1}},
		{{1, 1, 0},{0, 0}},
		{{1, 1, 1},{0, 1}},

        //BOTTOM
		{{0, 0, 0},{0, 0}},
		{{1, 0, 1},{1, 1}},
		{{1, 0, 0},{1, 0}},

		{{1, 0, 1},{1, 1}},
		{{0, 0, 0},{0, 0}},
		{{0, 0, 1},{0, 1}},
	};

    const float unit = 1.0f/6.0f;
    const float scale = 600;
    for(int i = 0; i < _temp.size(); ++i)
    {
        const int current = i/6;
        _temp[i].texture.u = unit*(current+_temp[i].texture.u);
        for(int j = 0; j < 3; ++j)
            _temp[i].position[j] = scale*(-0.5+_temp[i].position[j]);
    }

    m_mesh.Init(_temp);
    m_mesh.SetTexture(texture);
}

void CSkyBox::Draw(CBulkRenderer* renderer)
{
    m_mesh.SetPosition(renderer->getActiveCamera()->GetGlobalPosition());
    m_mesh.BulkDraw(renderer);
}

CMesh& CSkyBox::getMesh()
{
    return m_mesh;
}