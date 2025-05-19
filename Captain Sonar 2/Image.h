#pragma once

#include <SDL.h>
#include <iostream>
#include <string>
using namespace std;

class Image {
public:
    Image(string File)
        : ImageSurface{ SDL_LoadBMP(File.c_str()) } {
        if (!ImageSurface) {
            cout << "Failed to load image "
                << File << ":\n  " << SDL_GetError();
        }
    }

    ~Image() {
        SDL_FreeSurface(ImageSurface);
    }

    void Render(SDL_Surface* DestinationSurface) {
        SDL_BlitSurface(
            ImageSurface, nullptr,
            DestinationSurface, nullptr
        );
    }

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

private:
    SDL_Surface* ImageSurface{ nullptr };
};


