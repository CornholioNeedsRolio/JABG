#define SDL_MAIN_HANDLED
#define WINVER 0x05010000
#define _WIN32_WINNT 0x05010000
#ifdef _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "src/CGame.h"
#include "src/CDebugLevel.h"
#include "src/CCamera.h"


int main()
{
	CDebugLevel level;
	CGame game;
	game.Init();
	level.Load(&game);

	game.setLevel(&level);
	game.Run();
	return 0;
}