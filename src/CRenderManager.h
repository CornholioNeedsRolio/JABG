#ifndef CRenderManager_Header
#define CRenderManager_Header
#include <vector>
#include "CObject.h"
#include <glad/glad.h>

class CRenderManager
{
    std::vector<CObject*> m_renderList = {};
public:
    void Draw(const SDrawInfo& info, GLuint width, GLuint height);
    void addObjectToRender(CObject* object);
};

#endif