#include "CTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

void CTexture::Init(std::string filepath)
{
	unsigned char* data = stbi_load(filepath.c_str(), &m_width, &m_height, nullptr, STBI_rgb_alpha);

	if (!data) std::cerr << "Invalid path: " + filepath;
	InitData(data, GL_RGBA, m_width, m_height);

	stbi_image_free(data);
}

void CTexture::InitData(unsigned char* data, GLenum mode, int width, int height, GLenum type)
{
	if (m_init) return;
	m_init = 1;
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &m_TEX);
	glBindTexture(GL_TEXTURE_2D, m_TEX);

	if (data)
		glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, type, data);
	else 
		glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CTexture::Bind()
{
	if (m_init)
		glActiveTexture(GL_TEXTURE0),
		glBindTexture(GL_TEXTURE_2D, m_TEX);
}

const int& CTexture::getWidth()
{
	return m_width;
}

const int& CTexture::getHeight()
{
	return m_height;
}

const GLuint& CTexture::getGLID() const
{
	return m_TEX;
}