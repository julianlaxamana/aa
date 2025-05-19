#pragma once

#include<vector>
#include <SDL.h>
#include "Component.h"

class Rectangle1 : public Component {
public:

    SDL_Color baseColor{ 0, 0, 0, 255};

    Rectangle1(
        int x, int y, int w, int h,
        SDL_Color Color = { 0, 0, 0, 255 })
        : Rect{ x, y, w, h }, baseColor{ Color } {}

    virtual void Render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        SDL_RenderFillRect(renderer, &Rect);
    }

    void SetShapePosition(int xPos, int yPos)
    {
        Rect.x = xPos;
        Rect.y = yPos;
    }

    void SetColor(SDL_Color C) { baseColor = C; }

    bool IsWithinBounds(int x, int y) const {
        // Too far left
        if (x < Rect.x) return false;
        // Too far right
        if (x > Rect.x + Rect.w) return false;
        // Too high
        if (y < Rect.y) return false;
        // Too low
        if (y > Rect.y + Rect.h) return false;
        // Within bounds
        return true;
    }

    SDL_Rect Rect{ 0, 0, 50, 50 };
};

class Circle : public Component {
    void Render(SDL_Surface* Surface) override {
        printf("Rendering Circle\n");
    }
};


