#ifndef CSkyBox_Header
#define CSkyBox_Header
#include "Engine/GLRenderer/CMesh.h"
#include "BulkRenderer/CBulkRenderer.h"
#include <memory>

class CSkyBox
{
    CMesh m_mesh;
public:
    CSkyBox();
    void Init(std::shared_ptr<class CTexture> texture);
    void Draw(CBulkRenderer* renderer);
    CMesh& getMesh();
};

#endif