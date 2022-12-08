#ifndef CINPUTMANAGER_HEADER
#define CINPUTMANAGER_HEADER
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <memory>
#include <array>

enum Input_Mouse
{
	RIGHT_MOUSE,
	MIDDLE_MOUSE,
	LEFT_MOUSE
};
class CInputManager
{
	void pressKey();
	void releaseKey();
	int m_numKeys = 0;
	Uint8* m_keystates = nullptr;
	std::unique_ptr<Uint8[]> m_oldKeystates = nullptr;
	glm::ivec2 m_mousepos, m_mouserel;
	std::array<bool, LEFT_MOUSE+1> m_mouseButtons;
	std::array<bool, LEFT_MOUSE+1> m_mouseButtonsPrevious;
	bool shouldCenter = false;
public:
	CInputManager();
	bool Update(SDL_Window* _window);

	glm::ivec2 getMousePosition();
	glm::ivec2 getMouseRelative();
	void setMousePosition(glm::ivec2 _pos);
	void centerMouse();

	bool keyDown(SDL_Scancode key);
	bool keyPressed(SDL_Scancode key);
	bool keyReleased(SDL_Scancode key);

	bool mouseButtonDown(Input_Mouse key);
	bool mouseButtonPressed(Input_Mouse key);
	bool mouseButtonReleased(Input_Mouse key);
};

#endif