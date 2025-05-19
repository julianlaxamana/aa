#pragma once

#include<vector>
#include <SDL.h>

#include "Shapes.h"

class Text : public Rectangle {
public:
    Text(int posX, int posY, int rectWidth, int rectHeight)
        : Rect{ posX, posY, rectWwidth, rectHeight } {}

    void Render(SDL_Surface* Surface) {
        Rectangle::Render(Surface);
        // Render Text...
    }
};


