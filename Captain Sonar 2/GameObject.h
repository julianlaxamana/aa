#pragma once
#include<vector>
#include <SDL.h>
#include "Component.h"
#include <string>


class Vector2 {
public:
	
	Vector2(int newX = 100, int newY = 100) {
		x = newX;
		y = newY;
	}

	int x;
	int y;

};

class GameObject {

public:

	std::string name = "New GameObject";
	Vector2 position{ 100, 100 };
	Vector2 scale{ 100,100 };

	GameObject(std::string newName = "New GameObject", int xPos = 100, int yPos = 100, int xScale = 100, int yScale = 100) {

		name = newName;
		position.x = xPos;
		position.y = yPos;

		scale.x = xScale;
		scale.y = yScale;
	}

	/*virtual void Render()
	{
		printf("Rendering");
	} */

	void MovePosition(int xMoveBy, int yMoveBy)
	{
		position.x += xMoveBy;
		position.y += yMoveBy;
	}

	void SetPosition(int xMove, int yMove)
	{
		position.x = xMove;
		position.y = yMove;
	}

};

