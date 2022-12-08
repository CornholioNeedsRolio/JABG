#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "Engine/CGame.h"
#include "CDebugLevel.h"
#include <fenv.h>

#include <limits.h>
#if CHAR_BIT != 8
#error "Char must be 8 bits wide"
#endif

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