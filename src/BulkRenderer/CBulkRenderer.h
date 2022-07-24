#ifndef CBulkRenderer_Header
#define CBulkRenderer_Header
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>

struct SRenderObject
{
    glm::vec4 color;
    glm::mat4 renderMatrix;
    bool isWireframeMode = false;
    std::shared_ptr<class CTexture> texture = nullptr;
    class CMesh* mesh = nullptr;
};

struct SRenderBulk
{
    class CShader* shader = nullptr;
    std::vector<SRenderObject> renderObjects {};
};

using RenderOrder = std::map<int, std::vector<SRenderBulk>>;

class CBulkRenderer
{
    RenderOrder m_renderingList;
    class CCamera* m_camera = nullptr;
public:
    void AddToBulkRender(class CMesh* object, int priority = 0);
    void setActiveCamera(class CCamera* camera);
    CCamera* getActiveCamera();
    void RenderAll(bool print = false);
};

#endif