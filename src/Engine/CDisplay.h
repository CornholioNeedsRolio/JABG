#ifndef CDISPLAY_HEADER
#define CDISPLAY_HEADER

#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include "CInputManager.h"

struct sdl_window_deleter
{
	void operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }
};

class CDisplay
{
	SDL_Window *m_window = nullptr;
	SDL_GLContext m_context;
	bool m_running = false;
public:
	void Init(std::string title, int width, int height);
	void setResolution(int width, int height);
	bool Run(CInputManager& _inputmanager);
	~CDisplay();
};

#endif