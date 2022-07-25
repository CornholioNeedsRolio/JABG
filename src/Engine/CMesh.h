#ifndef CMESH_HEADER
#define CMESH_HEADER
#include "CObject.h"
#include "CCamera.h"
#include "CShader.h"
#include "CTexture.h"
#include <vector>
#include "glad/glad.h"

struct SVertex
{
	struct __pos
	{
		float x;
		float y;
		float z;
		float& operator[](int i)
		{
			switch(i)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
			}
			return x;
		}
	}position;
	
	struct __tex
	{
		float u;
		float v;
	}texture;

	struct __normal
	{
		float x = 0;
		float y = 0;
		float z = 0;
	}normal;
};

class CMesh : public CObject
{
	bool m_init = false;
	bool m_wireframe = false;
	int m_priority = 0;
	GLuint m_VAO, m_VBO, m_indices;
	GLenum m_type;
	std::shared_ptr<CShader> m_shader = nullptr;
	std::shared_ptr<CTexture> m_texture = nullptr;
	glm::vec4 m_color;
public:
	void Init(const std::vector<SVertex>& _data, GLenum type = GL_TRIANGLES);
	void InitCube(float size);
	void InitGrid(int sizeX, int sizeY);
	void Init2DRect(float x1, float y1, float x2, float y2);
	void InitRect(float x1, float y1, float z1, float x2, float y2, float z2);
	void InitDebugRect(float x1, float y1, float z1, float x2, float y2, float z2);
	void InitLine(float x1, float y1, float z1, float x2, float y2, float z2);

	void SetPriority(int priority);
	void SetTexture(std::shared_ptr<CTexture> _texture);
	void SetShader(std::shared_ptr<CShader> _shader);
	void SetWireframe(bool value);
	std::shared_ptr<CTexture> GetTexture() const;
	std::shared_ptr<CShader> GetShader() const;
	const glm::vec4& getColor() const;
	bool isWireframeMode() const;

	const bool& isInit();

	void setColor(glm::vec4 color);
	void setColor(glm::vec3 color);
	
	void Clear();
	void Draw(const SDrawInfo& info) override;
	void BulkDraw(class CBulkRenderer* renderer) override;


	void DrawGLSimple() const;
	
	~CMesh();
};

#endif