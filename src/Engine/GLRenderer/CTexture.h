#ifndef CTEXTURE_HEADER
#define CTEXTURE_HEADER
#include <string>
#include "glad/glad.h"

class CTexture
{
	bool m_init = false;
	GLuint m_TEX;
	GLint m_width, m_height;
public:
	void Init(std::string filepath);
	void InitData(unsigned char* data, GLenum mode, int width, int height, GLenum type = GL_UNSIGNED_BYTE);

	void Bind();
	const GLuint& getGLID() const;
	const int& getWidth();
	const int& getHeight();
};

#endif // !CTEXTURE_HEADER
