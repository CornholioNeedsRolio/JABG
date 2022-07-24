#include "CMesh.h"
#include "CDirectionalLight.h"
#include "CFileManager.h"
#include "BulkRenderer/CBulkRenderer.h"
#include <iostream>



void CMesh::Init(const std::vector<SVertex>& _data, GLenum type)
{
	if (!_data.size() || m_init)
		return;
	m_color = glm::vec4(1);
	m_type = type;

	m_init = 1;

	glCreateVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof _data[0], _data.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(SVertex), (GLvoid*)offsetof(SVertex, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(SVertex), (GLvoid*)offsetof(SVertex, texture));
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(SVertex), (GLvoid*)offsetof(SVertex, normal));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_indices = (GLuint)_data.size();
}


void CMesh::Init2DRect(float x1, float y1, float x2, float y2)
{
	std::vector<SVertex> _temp = {
		{{x1, y1, 0},{0, 0}},
		{{x1, y2, 0},{0, 1}},
		{{x2, y2, 0},{1, 1}},

		{{x2, y2, 0},{1, 1}},
		{{x2, y1, 0},{1, 0}},
		{{x1, y1, 0},{0, 0}}
	};
	Init(_temp);
}

void CMesh::InitCube(float size)
{
	std::vector<SVertex> _temp = {

		//FRONT
		{{size, size, size},{0, 0}},
		{{0, size, size},{1, 0}},
		{{0, 0, size},{1, 1}},

		{{0, 0, size},{1, 1}},
		{{size, 0, size},{0, 1}},
		{{size, size, size},{0, 0}},

		//BACK
		{{0, size, 0},{0, 0}},
		{{size, size, 0},{1, 0}},
		{{size, 0, 0},{1, 1}},

		{{size, 0, 0},{1, 1}},
		{{0, 0, 0},{0, 1}},
		{{0, size, 0},{0, 0}},

		//RIGHT
		{{size, size, 0},{0, 0}},
		{{size, size, size},{1, 0}},
		{{size, 0, size},{1, 1}},

		{{size, 0, size},{1, 1}},
		{{size, 0, 0},{0, 1}},
		{{size, size,  0},{0,0}},

		//LEFT
		{{0, size, size},{0, 0}},
		{{0, size, 0},{1, 0}},
		{{0, 0, 0},{1, 1}},

		{{0, 0, 0},{1, 1}},
		{{0, 0, size},{0, 1}},
		{{0, size, size},{0, 0}},

		//TOP
		{{size, size, 0},{0, 0}},
		{{0, size, 0},{1, 0}},
		{{0, size, size},{1, 1}},

		{{0, size, size},{1, 1}},
		{{size, size, size},{0, 1}},
		{{size, size, 0},{0, 0}},

		//BOTTOM
		{{0, 0, 0},{0, 0}},
		{{size, 0, 0},{1, 0}},
		{{size, 0, size},{1, 1}},

		{{size, 0, size},{1, 1}},
		{{0, 0, size},{0, 1}},
		{{0, 0, 0},{0, 0}}

	};
	Init(_temp);
}

void CMesh::InitGrid(int sizeX, int sizeY)
{
	sizeX *= 0.5;
	sizeY *= 0.5;
	std::vector<SVertex> vertices = {};
	for(float i = -sizeX; i <= sizeX; i+= 1)
	{
			vertices.push_back({ {i, 0, -(float)sizeY} });
			vertices.push_back({ {i, 0,  (float)sizeY} });

			vertices.push_back({ {-(float)sizeX, 0, i} });
			vertices.push_back({ { (float)sizeX, 0,  i} });
	}
	Init(vertices, GL_LINES);
}

void CMesh::InitRect(float x1, float y1, float z1, float x2, float y2, float z2)
{
	std::vector<SVertex> _temp = {

		//FRONT
		{{1, 1, 1},{0, 0}},
		{{0, 1, 1},{1, 0}},
		{{0, 0, 1},{1, 1}},

		{{0, 0, 1},{1, 1}},
		{{1, 0, 1},{0, 1}},
		{{1, 1, 1},{0, 0}},

		//BACK
		{{0, 1, 0},{0, 0}},
		{{1, 1, 0},{1, 0}},
		{{1, 0, 0},{1, 1}},

		{{1, 0, 0},{1, 1}},
		{{0, 0, 0},{0, 1}},
		{{0, 1, 0},{0, 0}},

		//RIGHT
		{{1, 1, 0},{0, 0}},
		{{1, 1, 1},{1, 0}},
		{{1, 0, 1},{1, 1}},

		{{1, 0, 1},{1, 1}},
		{{1, 0, 0},{0, 1}},
		{{1, 1,  0},{0,0}},

		//LEFT
		{{0, 1, 1},{0, 0}},
		{{0, 1, 0},{1, 0}},
		{{0, 0, 0},{1, 1}},

		{{0, 0, 0},{1, 1}},
		{{0, 0, 1},{0, 1}},
		{{0, 1, 1},{0, 0}},

		//TOP
		{{1, 1, 0},{0, 0}},
		{{0, 1, 0},{1, 0}},
		{{0, 1, 1},{1, 1}},

		{{0, 1, 1},{1, 1}},
		{{1, 1, 1},{0, 1}},
		{{1, 1, 0},{0, 0}},

		//BOTTOM
		{{0, 0, 0},{0, 0}},
		{{1, 0, 0},{1, 0}},
		{{1, 0, 1},{1, 1}},

		{{1, 0, 1},{1, 1}},
		{{0, 0, 1},{0, 1}},
		{{0, 0, 0},{0, 0}}

	};

	for (auto& val : _temp)
	{
		if (val.position.x)
			val.position.x = x2;
		else
			val.position.x = x1;
		
		if (val.position.y)
			val.position.y = y2;
		else
			val.position.y = y1;

		if (val.position.z)
			val.position.z = z2;
		else
			val.position.z = z1;

	}
	Init(_temp);
}

void CMesh::InitDebugRect(float x1, float y1, float z1, float x2, float y2, float z2)
{
	std::vector<SVertex> _temp =
	{
		{{x1, y1, z1},{0, 0}},
		{{x2, y1, z1},{0, 0}},

		{{x1, y1, z1},{0, 0}},
		{{x1, y1, z2},{0, 0}},

		{{x2, y1, z1},{0, 0}},
		{{x2, y1, z2},{0, 0}},

		{{x1, y1, z2},{0, 0}},
		{{x2, y1, z2},{0, 0}},

		{{x1, y2, z1},{0, 0}},
		{{x2, y2, z1},{0, 0}},

		{{x1, y2, z1},{0, 0}},
		{{x1, y2, z2},{0, 0}},

		{{x2, y2, z1},{0, 0}},
		{{x2, y2, z2},{0, 0}},

		{{x1, y2, z2},{0, 0}},
		{{x2, y2, z2},{0, 0}},

		{{x1, y1, z1},{0, 0}},
		{{x1, y2, z1},{0, 0}},

		{{x2, y1, z1},{0, 0}},
		{{x2, y2, z1},{0, 0}},

		{{x2, y1, z2},{0, 0}},
		{{x2, y2, z2},{0, 0}},

		{{x1, y1, z2},{0, 0}},
		{{x1, y2, z2},{0, 0}},
	};
	Init(_temp, GL_LINES);
}

void CMesh::InitLine(float x1, float y1, float z1, float x2, float y2, float z2)
{
	std::vector<SVertex> _temp =
	{
		{{x1, y1, z1},{0, 0}},
		{{x2, y2, z2},{0, 0}}
	};
	Init(_temp, GL_LINES);
}

void CMesh::SetTexture(std::shared_ptr<CTexture> _texture)
{
	m_texture = _texture;
}

void CMesh::SetShader(std::shared_ptr<CShader> _shader)
{
	m_shader = _shader;
}

void CMesh::SetWireframe(bool value)
{
	m_wireframe = value;
}

std::shared_ptr<CTexture> CMesh::GetTexture() const
{
	return m_texture;
}

std::shared_ptr<CShader> CMesh::GetShader() const
{
	if(!m_shader)
		return CFileManager::getDefaultShader();
	return m_shader;
}

const bool& CMesh::isInit()
{
	return m_init;
}

void CMesh::setColor(glm::vec4 color)
{
	m_color = color;
}

void CMesh::setColor(glm::vec3 color)
{
	m_color = glm::vec4(color, 1);
}

void CMesh::Clear()
{
	if (!m_init) return;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	m_init = 0;
}

void CMesh::Draw(const SDrawInfo& info)
{
	if (!m_init) return;

	if (!m_shader)
		m_shader = info.defaultShader;

	if (m_shader)
	{
		if (info.camera) 
		{
			m_shader->SetUniform(info.camera->getProjection(), "ProjectionMatrix");
			m_shader->SetUniform(getModelMatrix(nullptr), "ModelMatrix");
			m_shader->SetUniform(info.camera->getView(false), "ViewMatrix");
		}
		
		if (m_texture) m_texture->Bind();
		else glBindTexture(GL_TEXTURE_2D, 0);

		if(info.testlight)
		{
			info.testlight->configureShader(m_shader.get());
		}

		m_shader->SetUniform(m_color, "color");

		m_shader->SetUniform(m_texture != nullptr, "doesHaveTexture");
	}

	if (m_wireframe)
		glPolygonMode(GL_FRONT, GL_LINE),
		glPolygonMode(GL_BACK, GL_LINE),
		glDisable(GL_CULL_FACE);
	else
		glPolygonMode(GL_FRONT, GL_FILL),
		glPolygonMode(GL_BACK, GL_FILL),
		glEnable(GL_CULL_FACE);

	DrawGLSimple();
	glBindVertexArray(0);
}

void CMesh::SetPriority(int priority)
{
	m_priority = priority;
}

void CMesh::BulkDraw(CBulkRenderer* renderer)
{
	renderer->AddToBulkRender(this, m_priority);
}

bool CMesh::isWireframeMode() const
{
	return m_wireframe;
}

const glm::vec4& CMesh::getColor() const
{
	return m_color;
}

void CMesh::DrawGLSimple() const
{
	if (!m_init) return;
	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawArrays(m_type, 0, m_indices);
}

CMesh::~CMesh()
{
	Clear();
}
