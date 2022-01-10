#ifndef CDirectionalLight_Header
#define CDirectionalLight_Header
#include <glad/glad.h>
#include <cstdint>
#include "CTexture.h"
#include "CRenderManager.h"
#include "CCamera.h"
#include "CShader.h"

class CDirectionalLight : private CObject
{
	GLuint m_width, m_height, m_depthMapFBO;
	uint8_t m_init;
	CTexture m_depthBuffer;
	CCamera m_lightCamera;
	
	SDrawInfo m_info;
	glm::mat4 m_cache;
public:
	CDirectionalLight();
	~CDirectionalLight();
	void Init(int texWidth, int texHeight);
	void Draw(const SDrawInfo& info) override;
	
	void renderToMe(CRenderManager& manager, const SDrawInfo& info, const CCamera& mainCamera);

	void setAngle(float pitch, float yaw, float roll);
	glm::mat4 getLightMatrix();
	void bindDepthBuffer();
	void configureShader(CShader* shader);
	
	CTexture& getTexture();
};

#endif