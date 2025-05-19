#pragma once
#include<vector>
#include <SDL.h>
#include <stdio.h>
#include "Component.h"
#include"GameObject.h"
#include <string>


class Level {

public:

	std::string name = "New Level";
	std::vector<GameObject> gameObjects;

	SDL_Color backgroundColor{ 0, 0, 0 };

	Level(std::string newName = "New Level", int R = 0, int G = 0, int B = 0) {
		name = newName;
		backgroundColor.r = R;
		backgroundColor.g = G;
		backgroundColor.b = B;

	};

};