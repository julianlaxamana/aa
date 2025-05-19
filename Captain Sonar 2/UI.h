#pragma once

#include<vector>
#include <SDL.h>
#include "Grid.h"
#include "Shapes.h"

class MainMenu {
public:
    void HandleEvent(const SDL_Event& E) {};
    void Render(SDL_Surface* Surface) {};
};

class Sidebar {
public:
    void HandleEvent(const SDL_Event& E) {};
    void Render(SDL_Surface* Surface) {};
};

class Footer {
public:
    void HandleEvent(const SDL_Event& E) {};
    void Render(SDL_Surface* Surface) {};
};

class Button : public Rectangle1 {

public:
    Button(int x, int y, int w, int h, SDL_Color newCol = {0, 0, 0, 255}) : Rectangle1
    { x, y, w, h } {
        SetColor(newCol);
    }

    void HandleEvent(const SDL_Event& E) {
        if (E.type == SDL_MOUSEMOTION) {
            HandleMouseMotion(E.motion);
        }
        else if (E.type == SDL_MOUSEBUTTONDOWN) {
            HandleMouseButton(E.button);
        }
    }

    SDL_Color idleColor = { 35, 185, 100 };
    SDL_Color hoverColor = { 125, 65, 15 };

protected:
    virtual void HandleLeftClick() {}
    virtual void HandleRightClick() {}
    virtual void HandleMouseEnter() {
        SetColor({ hoverColor });
    }
    virtual void HandleMouseExit() {
        SetColor({ idleColor });
    }

private:
    void HandleMouseMotion(
        const SDL_MouseMotionEvent& E) {
        if (IsWithinBounds(E.x, E.y)) {
            HandleMouseEnter();
        }
        else {
            HandleMouseExit();
        }
    }
    void HandleMouseButton(
        const SDL_MouseButtonEvent& E) {
        if (IsWithinBounds(E.x, E.y)) {
            const Uint8 Button{ E.button };
            if (Button == SDL_BUTTON_LEFT) {
                HandleLeftClick();
            }
            else if (Button == SDL_BUTTON_RIGHT) {
                HandleRightClick();
            }
        }
    }
};

class UI {
public:

    UI() {
     /*   GridComponent.AddChild(
            std::make_shared<Rectangle>());
        GridComponent.AddChild(
            std::make_shared<Circle>()); */
    }

    void HandleEvent(const SDL_Event& E) {
        MyButton.HandleEvent(E);
    };

    void Render(SDL_Renderer* renderer) {
        MyButton.Render(renderer);
    };

    Button MyButton{ 50, 50, 50, 50, {75, 125, 15 } };

private:
    MainMenu MenuComponent;
    Sidebar SidebarComponent;
    Footer FooterComponent;
    Grid GridComponent;
};


