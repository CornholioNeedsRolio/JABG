#ifndef CBulkRenderer_Header
#define CBulkRenderer_Header
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include <string>

using RenderOrder = std::vector<std::pair<class CShader*, std::vector<std::pair<class CMesh*, std::pair<glm::mat4, glm::vec4>>>>>;

class CBulkRenderer
{
    std::vector<std::pair<class CMesh*, std::pair<glm::mat4, glm::vec4>>> m_renderingList;
    class CCamera* m_camera = nullptr;
    RenderOrder sortObjectForRendering();
public:
    void AddToBulkRender(class CMesh* object);
    void setActiveCamera(class CCamera* camera);
    CCamera* getActiveCamera();
    void RenderAll(bool print = false);
};

#endif