#pragma once

#include <cstdint>
#include <SDL.h>

namespace gb_emu
{
	class LCD
	{
	public:
		LCD();
		~LCD();
		void setColour(uint32_t r, uint32_t g, uint32_t b);
		inline void hblank() { currX = 0; ++currY; }
		inline void vblank() { currX = currY = 0; }
	private:
		SDL_Window * window;
		SDL_Renderer* renderer;
		uint8_t currX = 0, currY = 0;
	};
}