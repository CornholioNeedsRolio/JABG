#include "CBulkRenderer.h"
#include "../CMesh.h"
#include "../CShader.h"
#include "../CCamera.h"
#include "../CFileManager.h"
#include <iostream>


void CBulkRenderer::RenderAll()
{
    if(!m_camera) return;
    RenderOrder order = sortObjectForRendering();
    m_camera->getCollider().updatePlanes();
    for(int i = 0; i != order.size(); ++i)
    {
        CShader* shader = order[i].first;
        if(!shader)
            shader = CFileManager::getDefaultShader().get();
        shader->Use();
        shader->SetUniform(m_camera->getProjection(), "ProjectionMatrix");
		shader->SetUniform(m_camera->getView(), "ViewMatrix");
        glm::mat4 old_ModelMatrix;
        std::shared_ptr<CTexture> old_Texture = nullptr;
        glm::vec3 old_Color;
        bool wireframe = false;
        for(int j = 0; j != order[i].second.size(); ++j)
        {
            CMesh* mesh = order[i].second[j];
            glm::mat4 objectModelMat = mesh->getModelMatrix();
            std::shared_ptr<CTexture> current_Texture = mesh->GetTexture();
            
            if(old_ModelMatrix != objectModelMat)
		        shader->SetUniform(old_ModelMatrix = objectModelMat, "ModelMatrix");
            
            if(old_Texture != current_Texture)
            {
                if(current_Texture == nullptr)
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    shader->SetUniform(false, "doesHaveTexture");
                }
                else
                {
                    (old_Texture = current_Texture)->Bind();
                    shader->SetUniform(true, "doesHaveTexture");
                }
            }
            if(old_Color != mesh->getColor())
                shader->SetUniform(old_Color = mesh->getColor(), "color");
            
            if(wireframe != mesh->isWireframeMode())
            {
                if ((wireframe = mesh->isWireframeMode()))
                    glPolygonMode(GL_FRONT, GL_LINE),
                    glPolygonMode(GL_BACK, GL_LINE),
                    glDisable(GL_CULL_FACE);
                else
                    glPolygonMode(GL_FRONT, GL_FILL),
                    glPolygonMode(GL_BACK, GL_FILL),
                    glEnable(GL_CULL_FACE);
            }

            mesh->DrawGLSimple();
        }
    }
    m_renderingList.clear();
}

CCamera* CBulkRenderer::getActiveCamera()
{
    return m_camera;
}

void CBulkRenderer::setActiveCamera(CCamera* camera)
{
    m_camera = camera;
}

RenderOrder CBulkRenderer::sortObjectForRendering()
{
    RenderOrder order;
    for(int i = 0; i != m_renderingList.size(); ++i)
    {
        CShader* shader = m_renderingList[i]->GetShader().get();
        int index = -1;

        //searching for shader
        for(int j = 0; j < order.size(); ++j)
            if(order[j].first == shader)
            {
                index = j;
                break;
            }
        //if none found add it here
        if(index == -1)
        {
            index = order.size();
            order.push_back(std::make_pair(shader, std::vector<class CMesh*>()));
        }
        //add to the list
        order[index].second.push_back(m_renderingList[i]);
    }
    return order;
}

void CBulkRenderer::AddToBulkRender(CMesh* object)
{
    m_renderingList.push_back(object);
}