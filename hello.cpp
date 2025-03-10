#include "sdl.hpp"

#include <iostream>

int main() {
	stylizer::sdl.init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_Window* window = SDL_CreateWindow("Hello World", 800, 600, 0);

	while(stylizer::sdl.poll_until_or_done(stylizer::target_fps_time(60))) {
		std::cout << "loop" << std::endl;
	}

	SDL_DestroyWindow(window);
}