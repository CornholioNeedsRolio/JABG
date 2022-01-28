#include "CBulkRenderer.h"
#include "../CMesh.h"
#include "../CShader.h"
#include "../CCamera.h"
#include "../CFileManager.h"
#include <iostream>


void CBulkRenderer::RenderAll(bool print)
{
    if(!m_camera) return;
    RenderOrder order = sortObjectForRendering();
    m_camera->getCollider().updatePlanes();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int num = 0;
    for(int i = order.size()-1; i >= 0; --i)
    {
        CShader* shader = order[i].first;
        if(!shader)
            shader = CFileManager::getDefaultShader().get();
        shader->Use();
        shader->SetUniform(m_camera->getProjection(), "ProjectionMatrix");
		shader->SetUniform(m_camera->getView(true), "ViewMatrix");
        glm::mat4 old_ModelMatrix;
        std::shared_ptr<CTexture> old_Texture = nullptr;
        glm::vec4 old_Color(-1);
        bool wireframe = false;
        for(int j = 0; j < order[i].second.size(); ++j)
        {
            CMesh* mesh = order[i].second[j].first;
            glm::mat4 objectModelMat = order[i].second[j].second.first;
            glm::vec4 color = order[i].second[j].second.second;
            std::shared_ptr<CTexture> current_Texture = mesh->GetTexture();
            
            if(old_ModelMatrix != objectModelMat)
		        shader->SetUniform(old_ModelMatrix = objectModelMat, "ModelMatrix");
            
            if(current_Texture == nullptr)
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                shader->SetUniform(false, "doesHaveTexture");
            }
            else if(old_Texture != current_Texture)
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
            if(old_Color != color)
                shader->SetUniform(old_Color = color, "color");
            
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
            //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            mesh->DrawGLSimple();
            ++num;
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
        CShader* shader = m_renderingList[i].first->GetShader().get();
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
            order.push_back(std::make_pair(shader, std::vector<std::pair<class CMesh*, std::pair<glm::mat4, glm::vec4>>>()));
        }
        //add to the list
        order[index].second.push_back(m_renderingList[i]);
    }
    return order;
}

void CBulkRenderer::AddToBulkRender(CMesh* object)
{
    m_renderingList.push_back(std::make_pair(object, std::make_pair(object->getModelMatrix(m_camera), object->getColor())));
}