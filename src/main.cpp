#include "../include/vm.hpp"
#include <SDL.h>

int main(int argc, char *args[])
{
	SDL_Init(SDL_INIT_VIDEO);

	gb_emu::VM vm;
	vm.run();

	SDL_Quit();
	return 0;
}