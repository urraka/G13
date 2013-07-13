#include "g13.h"
#include "../Game/Game.h"
#include "../Game/Debugger.h"

Game *game = 0;

namespace g13 {

void initialize()
{
	#ifdef DEBUG
		dbg = new Debugger();
	#endif

	game = new Game();
	game->initialize();
}

void display()
{
	game->input();
	game->draw();
	game->update();
}

void terminate()
{
	delete game;
}

} // g13
