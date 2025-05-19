#pragma once
#include <SDL.h>
#include <memory>
#include "Component.h"
#include "GameObject.h"
#include "Shapes.h"

struct GridSlot : GameObject
{
    bool occupiedByLand = false;
    bool occupiedByPlayer = false;
    bool occupiedByMine = false;
    int playerOwner = 0;

    Rectangle borderRect{ 100,100,100,100 };
    Rectangle innerRect{ 100,100,100,100 };

    GridSlot(std::string newName = "New Gridslot", int xPos = 100, int yPos = 100, int xScale = 25, int yScale = 25) {

        name = newName;
        position.x = xPos;
        position.y = yPos;

        scale.x = xScale;
        scale.y = yScale;

        Rectangle newBorderRect(xPos, yPos, xScale, yScale);
        Rectangle newInnterRect(xPos+2, yPos+2, xScale - 4, yScale-4);

        borderRect = newBorderRect;
        borderRect.SetColor({ 27, 69, 124 });
        innerRect = newInnterRect;
        innerRect.SetColor({ 136,225,67 });
    };

    virtual void RenderSlot(SDL_Renderer* renderer) {
        borderRect.Render(renderer);
        if (occupiedByMine) innerRect.SetColor({ 228, 44, 44 });
        else if (occupiedByLand) innerRect.SetColor({ 100, 185, 125 });
        else innerRect.SetColor({ 148, 194, 255 });
        innerRect.Render(renderer);
    }

};

class Grid {
public:

    int rowSize = 10;
    int columnSize = 10;

    int screenWidth = 1920;
    int screenHeight = 1080;
    int gridSlotScale = 32;

    int numSectors = 4;

    std::vector<std::vector<GridSlot>> grid;

    Grid(int newRowSize = 10, int newColumnSize = 10, int newScreenWidth = 1280, int newScreenHeight = 720)
    {
        rowSize = newRowSize;
        columnSize = newColumnSize;
        screenWidth = newScreenWidth;
        screenHeight = newScreenHeight;
        gridSlotScale = 32;

        for (int i = 0; i < rowSize; i++)
        {
            std::vector<GridSlot> newRow;
            grid.push_back(newRow);
            for (int j = 0; j < columnSize; j++)
            {
                Vector2 baseOffset((screenWidth/2)-(gridSlotScale*(columnSize/2)),(screenHeight/2)-(gridSlotScale*(rowSize/2)));
                GridSlot newSlot("New GridSlot", baseOffset.x + (i * gridSlotScale), baseOffset.y + (j * gridSlotScale), gridSlotScale, gridSlotScale);
                 grid.at(i).push_back(newSlot);
            }
        }
    };

    bool CheckSectorForPlayer(int sector)
    {
        for (int i = 0; i < rowSize; i++)
        {
            for (int j = 0; j < columnSize; j++)
            {
                if (grid.at(i).at(j).occupiedByPlayer)
                {
                    if (sector == 1 && i < rowSize / 2 && j < columnSize / 2)  return true;
                    else if (sector == 2 && i >= rowSize / 2 && j < columnSize / 2) return true;
                    else if (sector == 3 && i < rowSize / 2 && j >= columnSize / 2) return true;
                    else if (sector == 4 && i >= rowSize / 2 && j >= columnSize / 2) return true;
                }
            }
        }

        return false;
    }

    void RenderGrid(SDL_Renderer* renderer)
    {
        for (int i = 0; i < rowSize; i++)
        {
            for (int j = 0; j < columnSize; j++)
            {
                grid.at(i).at(j).RenderSlot(renderer);
            }
        }
    }

    Vector2 GridSlotPosition(int row, int column)
    {
        float x = (screenWidth / 2) - (gridSlotScale * (columnSize / 2));
        float y = (screenHeight / 2) - (gridSlotScale * (rowSize / 2));

        int rowSlot = 4;
        int columnSlot = 4;

        if (row != 0) rowSlot += (32*row);
        if (column != 0) columnSlot += (32*column);

        Vector2 returnVector(x + rowSlot, y + columnSlot);

        return returnVector;
    }
};


