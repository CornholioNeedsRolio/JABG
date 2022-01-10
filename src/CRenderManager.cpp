#include "CRenderManager.h"
#include "CCamera.h"

void CRenderManager::Draw(const SDrawInfo& info, GLuint width, GLuint height)
{
    glViewport(0, 0, width, height);
    info.camera->getCollider().updatePlanes();
    for(int i = 0; i != m_renderList.size(); ++i)
        m_renderList[i]->Draw(info);
}

void CRenderManager::addObjectToRender(CObject* object)
{
    m_renderList.push_back(object);
}