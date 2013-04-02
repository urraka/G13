#include <System/System.h>
#include <Game/Game.h>

int main(int argc, char *argv[])
{
	Application app(argc, argv);

	app.launched(Game::launch);
	app.terminate(Game::terminate);

	return app.run();
}

#if defined(WIN32)
	#include <windows.h>

	int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return main(0, 0);
	}
#endif
