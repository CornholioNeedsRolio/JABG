#ifndef CINPUTMANAGER_HEADER
#define CINPUTMANAGER_HEADER
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
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
	Uint8* m_keystates = nullptr;
	glm::ivec2 m_mousepos, m_mouserel;
	std::array<bool, LEFT_MOUSE+1> m_mouseButtons;
	bool shouldCenter = false;
public:
	bool Update(SDL_Window* _window);

	bool mouseButtonDown(Input_Mouse key);
	glm::ivec2 getMousePosition();
	glm::ivec2 getMouseRelative();
	void setMousePosition(glm::ivec2 _pos);
	void centerMouse();

	bool keyDown(SDL_Scancode key);
};

#endif