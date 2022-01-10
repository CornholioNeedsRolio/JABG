#ifndef CBulkRenderer_Header
#define CBulkRenderer_Header
#include <unordered_map>
#include <vector>
#include <string>

using RenderOrder = std::vector<std::pair<class CShader*, std::vector<class CMesh*>>>;

class CBulkRenderer
{
    std::vector<class CMesh*> m_renderingList;
    class CCamera* m_camera = nullptr;
    RenderOrder sortObjectForRendering();
public:
    void AddToBulkRender(class CMesh* object);
    void setActiveCamera(class CCamera* camera);
    CCamera* getActiveCamera();
    void RenderAll();
};

#endif