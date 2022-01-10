#include "CFileManager.h"

std::unordered_map<std::string, std::shared_ptr<CTexture>> CFileManager::m_textures;
std::unordered_map<std::string, std::shared_ptr<CShader>> CFileManager::m_shader;
std::shared_ptr<CShader> CFileManager::m_defaultShader = nullptr;

std::shared_ptr<CTexture> CFileManager::getTexture(std::string file)
{
/*
	for(auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		if (it->second.use_count() == 1)
			it = m_textures.erase(it);
		else
			it++;
	}*/

	auto it = m_textures.find(file);
	std::shared_ptr<CTexture> texture_temp = std::make_shared<CTexture>();
	texture_temp->Init(file);
	if (it == m_textures.end())
		m_textures.insert(std::pair<std::string, std::shared_ptr<CTexture>>(file, texture_temp));
	else
		return it->second;
	
	return texture_temp;
}

std::shared_ptr<CShader> CFileManager::getShader(std::string file)
{
/*
	for (auto it = m_shader.begin(); it != m_shader.end(); it++)
	{
		if (it->second.use_count() == 1)
			it = m_shader.erase(it);
		else
			it++;
	}
*/

	auto it = m_shader.find(file);
	std::shared_ptr<CShader> shader_temp = std::make_shared<CShader>();
	shader_temp->Init(file);
	if (it == m_shader.end())
		m_shader.insert(std::pair<std::string, std::shared_ptr<CShader>>(file, shader_temp));
	else
		return it->second;
	
	return shader_temp;
}

std::shared_ptr<CShader> CFileManager::getDefaultShader()
{
	if(m_defaultShader)
		return m_defaultShader;
	return m_defaultShader = CFileManager::getShader("./res/DefaultShader/DefaultShader");
}