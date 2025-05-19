#pragma once
#include<vector>
#include <SDL.h>
#include "GameObject.h"
#include "Shapes.h"
#include <string>

struct UsableItem {

	string name = "DefaultName";
	int currentCharge = 0;
	int requiredCharge = 0;
	bool isCharging = false;

	UsableItem(std::string newName = "DefaultName", int newRequiredCharge = 3)
	{
		name = newName;
		requiredCharge = newRequiredCharge;
	}
};

class Player : public GameObject 
{

public:

	Vector2 slotPosition;
	int currentHealth = 0;
	int maxHealth = 5;
	int playerNumber = 0;

	UsableItem mineItem {"Mine", 3};
	UsableItem torpedoItem{ "Torpedo", 3};
	UsableItem sonarItem{ "Sonar", 3};
	UsableItem droneItem{ "Drone", 4};

	// Add Texture at some point.

	//NOTE, THIS IS SLOTSIZE
	Player(std::string newName = "New Player", int xPos = 100, int yPos = 100, int xScale = 100, int yScale = 100) {

		name = newName;
		position.x = xPos;
		position.y = yPos;

		scale.x = xScale;
		scale.y = yScale;

		Rectangle1 wanted{ position.x, position.y, scale.x, scale.y };

		playerRepresentative = wanted;

		currentHealth = maxHealth;
	}

	void operator=(const Player& other)
	{
		name = other.name;
		position.x = other.position.x;
		position.y = other.position.y;
		slotPosition = other.slotPosition;

		scale.x = other.scale.x;
		scale.y = other.scale.y;

		playerRepresentative = other.playerRepresentative;
		playerNumber = other.playerNumber;

		currentHealth = other.currentHealth;
		mineItem = other.mineItem;
		torpedoItem = other.torpedoItem;
		sonarItem = other.sonarItem;
		droneItem = other.droneItem;
	}

	Rectangle1 playerRepresentative{ position.x, position.y, scale.x, scale.y };

	 void Render(SDL_Renderer* renderer, bool currentTurn = false) {
		playerRepresentative.SetShapePosition(position.x, position.y);
		if (currentTurn) playerRepresentative.baseColor = { 0, 75, 215 };
		else playerRepresentative.baseColor = { 148, 194, 255};

		playerRepresentative.Render(renderer);
	}
};

