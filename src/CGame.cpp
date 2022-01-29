#include "CGame.h"
#include "CLevel.h"
#include <chrono>

CGame* CGame::m_instance = nullptr;

void CGame::Init(std::string title)
{
	m_display.Init(title, 640, 480);
	m_instance = this;
}

void CGame::setResolution(int width, int height)
{
	m_display.setResolution(width, height);
}

void CGame::Run()
{
	auto begin = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();

	while(m_display.Run(m_inputmanager))
	{
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsedTime = end - begin;
		begin = end;
		m_deltaTime = elapsedTime.count();
		if (m_currentLevel && m_deltaTime > 0 && m_deltaTime <= 1)
		{
			m_currentLevel->Run(m_inputmanager, m_deltaTime);
			m_currentLevel->Draw();
			++m_tickNum;
		}
	}
}

CGame* CGame::getInstance()
{
	return m_instance;
}

unsigned int CGame::getTickNum() const
{
	return m_tickNum;
}

void CGame::setLevel(CLevel* level)
{
	m_currentLevel = level;
}
