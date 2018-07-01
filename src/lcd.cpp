#include "..\include\lcd.hpp"

namespace gb_emu
{
	LCD::LCD()
	{
		window = SDL_CreateWindow("GB_EMU", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		SDL_SetRenderDrawColor(renderer, 128, 0, 0, SDL_ALPHA_OPAQUE);
	}

	LCD::~LCD()
	{
		SDL_DestroyWindow(window);
	}

	void LCD::setColour(uint32_t r, uint32_t g, uint32_t b)
	{
		// Set colours

		// Increment pixel position
		++currX;

		if(currX > 160) {
			// Fire hblank interrupt?
		}
		if(currY > 144) {
			// Fire vblank interrupt?
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
		}
	}
}
