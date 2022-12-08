#ifndef CFILEMANAGER_HEADER
#define CFILEMANAGER_HEADER
#include <string>
#include <unordered_map>
#include <memory>
#include "Engine/GLRenderer/CTexture.h"
#include "Engine/GLRenderer/CShader.h"


class CFileManager
{
	static std::unordered_map<std::string, std::shared_ptr<CTexture>> m_textures;
	static std::unordered_map<std::string, std::shared_ptr<CShader>> m_shader;
	static std::shared_ptr<CShader> m_defaultShader;
	CFileManager();
public:
	static std::shared_ptr<CTexture> getTexture(std::string file);
	static std::shared_ptr<CShader> getShader(std::string file);
	static std::shared_ptr<CShader> getDefaultShader();
};

#endif
