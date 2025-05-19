#pragma once

#include <SDL.h>

class Window {
public:
    Window(int newWindowWidth = 700, int newWindowHeight = 300) {
        SDLWindow = SDL_CreateWindow(
            "My Program", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, newWindowWidth, newWindowHeight, 0);
    }

    SDL_Surface* GetSurface() {
        return SDL_GetWindowSurface(SDLWindow);
    }

    void Render() {
        SDL_FillRect(GetSurface(), nullptr,
            SDL_MapRGB(
                GetSurface()->format, 102, 99, 97)
        );

    }


    void Update() {
        SDL_UpdateWindowSurface(SDLWindow);
    }

    ~Window() {
        SDL_DestroyWindow(SDLWindow);
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    SDL_Window* SDLWindow;
};
