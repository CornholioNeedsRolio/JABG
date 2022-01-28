#include "CInputManager.h"
#include <iostream>

void CInputManager::pressKey()
{
	m_keystates = (Uint8*)SDL_GetKeyboardState(&m_numKeys);
}

void CInputManager::releaseKey()
{
	if(m_numKeys)
	{	
		if(!m_oldKeystates) 
			m_oldKeystates = std::make_unique<Uint8[]>(m_numKeys);

		for(int i = 0; i < m_numKeys; ++i)
			m_oldKeystates[i] = m_keystates[i];
	}
}

bool CInputManager::keyPressed(SDL_Scancode key)
{
	if(!m_keystates || !m_oldKeystates) return false;
	return !m_oldKeystates[key] && m_keystates[key];
}

bool CInputManager::keyReleased(SDL_Scancode key)
{
	if(!m_keystates || !m_oldKeystates) return false;
	return m_oldKeystates[key] && !m_keystates[key];
}

bool CInputManager::Update(SDL_Window* _window)
{
	SDL_Event event;
	m_mouserel *= 0;
	releaseKey();
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;
			break;
		case SDL_KEYDOWN:
			pressKey();
			break;
		case SDL_KEYUP:
			pressKey();
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				m_mouseButtons[LEFT_MOUSE] = true;
				break;
			case SDL_BUTTON_RIGHT:
				m_mouseButtons[RIGHT_MOUSE] = true;
				break;
			case SDL_BUTTON_MIDDLE:
				m_mouseButtons[MIDDLE_MOUSE] = true;
				break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				m_mouseButtons[LEFT_MOUSE] = false;
				break;
			case SDL_BUTTON_RIGHT:
				m_mouseButtons[RIGHT_MOUSE] = false;
				break;
			case SDL_BUTTON_MIDDLE:
				m_mouseButtons[MIDDLE_MOUSE] = false;
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			m_mousepos.x = event.motion.x;
			m_mousepos.y = event.motion.y;

			m_mouserel.x = event.motion.xrel;
			m_mouserel.y = event.motion.yrel;
			break;
		}
	}


	return true;
}

bool CInputManager::mouseButtonDown(Input_Mouse key)
{
	return m_mouseButtons[key];
}

glm::ivec2 CInputManager::getMousePosition()
{
	return m_mousepos;
}

glm::ivec2 CInputManager::getMouseRelative()
{
	return m_mouserel;
}

void CInputManager::setMousePosition(glm::ivec2 _pos)
{
	SDL_WarpMouseGlobal(_pos.x, _pos.y);
}

void CInputManager::centerMouse()
{
	shouldCenter = true;
}

bool CInputManager::keyDown(SDL_Scancode key)
{
	if(m_keystates)
		return m_keystates[key];
	return false;
}
