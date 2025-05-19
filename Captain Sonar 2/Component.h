#pragma once

#include<vector>
#include <SDL.h>

class Component {
public:
	virtual void HandleEvent(const SDL_Event& E) {}
	virtual void Render(SDL_Surface* Surface) {}
};


