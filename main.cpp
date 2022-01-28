#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
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