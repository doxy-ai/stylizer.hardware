#pragma once

#include <chrono>
#include <cmath>

#include "SDL3/SDL.h"

#include "event.hpp"

namespace stylizer {
	static struct SDL_Initializer {
		SDL_InitFlags initialized = false;
		event<SDL_Event> event_handler;

		bool init(SDL_InitFlags flags) {
			initialized |= flags;
			return SDL_InitSubSystem(flags);
		}

		void quit(SDL_InitFlags flags) {
			initialized &= ~flags;
			SDL_QuitSubSystem(flags);
		}

		~SDL_Initializer() {
			if(initialized)
				SDL_Quit();
		}

#define STYLIZER_POLL_IMPL {\
	event_handler(e);\
\
	if(e.type == SDL_EVENT_QUIT)\
		return false;\
}

		bool poll() {
			if(!(initialized & SDL_INIT_EVENTS)) init(SDL_INIT_EVENTS);

			SDL_Event e;
			if(SDL_PollEvent(&e)) STYLIZER_POLL_IMPL;
			return true;
		}

		bool poll_all() {
			if(!(initialized & SDL_INIT_EVENTS)) init(SDL_INIT_EVENTS);

			SDL_Event e;
			while(SDL_PollEvent(&e)) STYLIZER_POLL_IMPL;
			return true;
		}

		template<typename Clock, typename Duration>
		bool poll_until(const std::chrono::time_point<Clock, Duration>& time, std::chrono::milliseconds no_event_sleep_time = {1}) {
			if(!(initialized & SDL_INIT_EVENTS)) init(SDL_INIT_EVENTS);

			SDL_Event e;
			while(Clock::now() < time)
				if(SDL_PollEvent(&e)) { STYLIZER_POLL_IMPL; }
				else SDL_Delay(no_event_sleep_time.count());
			return true;
		}

		template<typename Clock, typename Duration>
		bool poll_until_or_done(const std::chrono::time_point<Clock, Duration>& time) {
			if(!(initialized & SDL_INIT_EVENTS)) init(SDL_INIT_EVENTS);

			SDL_Event e;
			while(Clock::now() < time && SDL_PollEvent(&e)) STYLIZER_POLL_IMPL;
			return true;
		}

#undef STYLIZER_POLL_IMPL

	} sdl;

	/**
	 * Determines the time point needed to wait until to hit a target FPS
	 *
	 * @param target_fps target FPS we are trying to maintain
	 * @param start time the frame started at (or now if not provided)
	 * @return time point representing when the frame needs to finish to hit the target FPS
	 */
	template<typename Clock, typename Duration>
	auto target_fps_time(float target_fps, const std::chrono::time_point<Clock, Duration>& start) {
		return start + std::chrono::milliseconds((size_t)std::round(1000 / target_fps));
	}
	inline auto target_fps_time(float target_fps) {
		return target_fps_time(target_fps, std::chrono::high_resolution_clock::now());
	}
}