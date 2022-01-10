#include "CDirectionalLight.h"
#include "CFileManager.h"
#include "CFrustumCollider.h"

CDirectionalLight::CDirectionalLight() : m_lightCamera()
{
	m_init = 0;
	setAngle(0.f, 0.f, 0.f);
}

CDirectionalLight::~CDirectionalLight()
{
	if (m_init)
		glDeleteFramebuffers(1, &m_depthMapFBO);
}

void CDirectionalLight::Init(int texWidth, int texHeight)
{
	m_width = texWidth;
	m_height = texHeight;
	m_init = 1;
	//m_lightCamera.MakeOrtho(-m_width*0.5f, -m_height*0.5f, m_width*0.5f, m_height*0.5f, -500.f, 500.f);
	m_lightCamera.MakeOrtho(-20.f, 20.f, -20.f, 20.f, -300.f, 300.f);

	//m_info.camera = &m_lightCamera;
	//m_info.defaultShader = CFileManager::getDefaultShader();

	glGenFramebuffers(1, &m_depthMapFBO);
	m_depthBuffer.InitData(nullptr, GL_DEPTH_COMPONENT, m_width, m_height, GL_FLOAT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer.getGLID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	m_lightCamera.makeMesh();
}

void CDirectionalLight::Draw(const SDrawInfo& info)
{
	m_lightCamera.Draw(info);
}

void CDirectionalLight::renderToMe(CRenderManager& manager, const SDrawInfo& info, const CCamera& mainCamera)
{
	
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	m_lightCamera.SetPosition(mainCamera.GetGlobalPosition());
	
	m_info = info;
	m_info.camera = &m_lightCamera;
	m_info.testlight = nullptr;
	
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	manager.Draw(m_info, m_width, m_height);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CDirectionalLight::configureShader(CShader* shader)
{
	shader->SetUniform(getLightMatrix(), "DirLightMatrix");
	glm::mat4 map = glm::inverse(m_lightCamera.getModelMatrix());
	shader->SetUniform((glm::vec3(map[2]) * glm::vec3(1, 1, -1)), "lightDirection");
	bindDepthBuffer();
}

void CDirectionalLight::setAngle(float pitch, float yaw, float roll)
{
	m_lightCamera.SetRotation({pitch, yaw, roll});
	//m_cache = m_lightCamera.getProjection() * m_lightCamera.getView();
}

glm::mat4 CDirectionalLight::getLightMatrix()
{
	return m_lightCamera.getProjection() * m_lightCamera.getView();
}

void CDirectionalLight::bindDepthBuffer()
{
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, m_depthBuffer.getGLID());
}
CTexture& CDirectionalLight::getTexture()
{
	return m_depthBuffer;
}