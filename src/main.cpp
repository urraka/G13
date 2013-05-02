#include "System/Application.h"
#include "Game/Game.h"

int main(int argc, char *argv[])
{
	Application app(argc, argv);

	app.launched(Game::launch);
	app.terminate(Game::terminate);

	return app.run();
}
