#define SDL_MAIN_HANDLED
#include "Core/Application.h"
#include <iostream>

auto main(int argc, char** argv) -> int
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << SDL_GetError() << std::endl;
	}
	return acc3d::Core::Application::Get().Start();
}
