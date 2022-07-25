#include "CBulkRenderer.h"
#include "Engine/CMesh.h"
#include "Engine/CShader.h"
#include "Engine/CCamera.h"
#include "Engine/CFileManager.h"
#include <iostream>


void CBulkRenderer::RenderAll(bool print)
{

    if(!m_camera) return;
    m_camera->getCollider().updatePlanes();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 old_ModelMatrix; glm::vec4 old_Color(-1); bool wireframe = false;
    for (auto const& [key, val] : m_renderingList)
    {
        for(auto const& bulk : val)
        {
            CShader* shader = bulk.shader;
            if(!shader)   shader = CFileManager::getDefaultShader().get();
            shader->Use();
            if(shader->GetShaderSettings())
                shader->GetShaderSettings()->UpdateUniforms(m_camera);

            shader->SetUniform(m_camera->getProjection(), "ProjectionMatrix");
            shader->SetUniform(m_camera->getView(true), "ViewMatrix");
            std::shared_ptr<CTexture> old_Texture = nullptr;
            for(auto const& renderObject : bulk.renderObjects)
            {
                CMesh* mesh = renderObject.mesh;
                glm::mat4 objectModelMat = std::move(renderObject.renderMatrix);
                glm::vec4 color = std::move(renderObject.color);
                std::shared_ptr<CTexture> current_Texture = std::move(renderObject.texture);

                if(old_ModelMatrix != objectModelMat) shader->SetUniform(old_ModelMatrix = objectModelMat, "ModelMatrix");
            
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
            }
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

void CBulkRenderer::AddToBulkRender(CMesh* object, int priority)
{
    SRenderObject currentMesh;
    currentMesh.color = object->getColor();
    currentMesh.renderMatrix = object->getModelMatrix(m_camera);
    currentMesh.isWireframeMode = object->isWireframeMode();
    currentMesh.mesh = object;
    currentMesh.texture = object->GetTexture();

    CShader* shader = object->GetShader().get();
    SRenderBulk bulk;
    bulk.shader = shader;
    bulk.renderObjects.push_back(currentMesh);

    auto iter = m_renderingList.find(priority);
    if(iter != m_renderingList.end())
    {
        for(auto it = iter->second.begin(); it != iter->second.end(); ++it)
            if(it->shader == shader)
            {
                it->renderObjects.push_back(currentMesh);
                return;
            }
        iter->second.push_back(bulk);
        return;
    }
    
    m_renderingList.insert(
        {
            priority,  
            std::vector<SRenderBulk>{{std::move(bulk)}}
        }
    );
}