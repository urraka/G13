#include "System/Application.h"
#include "Game/Game.h"

int main(int argc, char *argv[])
{
	Application app(argc, argv);

	app.launchCallback(Game::launch);
	app.terminateCallback(Game::terminate);

	return app.run();
}
