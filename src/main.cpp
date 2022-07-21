#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "CGame.h"
#include "CDebugLevel.h"
#include "CCamera.h"
#include <fenv.h>

int main()
{
//	feenableexcept(FE_INVALID);
	CDebugLevel level;
	CGame game;
	game.Init();
	level.Load(&game);

	game.setLevel(&level);
	game.Run();
	return 0;
}