#include <pch.h>
#include <Game/Game.h>

Game game;

#if !defined(IOS)

	int main()
	{
		if (game.init())
			game.loop();

		game.terminate();
		std::cin.ignore();

		return 0;
	}

	#if defined(WIN32)

		#include <windows.h>

		int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
		{
			return main();
		}

	#endif

#endif