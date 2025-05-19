#include <SDL.h>
#include <stdio.h>
#include <SDL_ttf.h>
#include "Window.h"
#include "UI.h"
#include "Image.h"
#include <iostream>
#include "GameObject.h"
#include "Level.h"
#include "Grid.h"
#include "Player.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

using namespace std;

extern const int SCREEN_WIDTH = 1280;
extern const int SCREEN_HEIGHT = 720;

int IMAGE_WIDTH = 25;
int IMAGE_HEIGHT = 25;

Level menuLevel("MainMenu", 0, 100, 85);
Level gameLevel("Game Scene", 50, 150, 100);


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window

//The image we will load and show on the screen
SDL_Surface* gOut = NULL;

SDL_Renderer* renderer = NULL;

SDL_Texture* garFeldiTexture = NULL;
SDL_Color currentColor;

TTF_Font* gFont = NULL;

SDL_Texture* textTexture;

enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

void imguiInit()
{


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(gWindow, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	printf("hi");
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		TTF_Init();
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(gWindow, -1, 0);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface

			SDL_SetRenderDrawColor(renderer, 102, 99, 97, 225);
			SDL_UpdateWindowSurface(gWindow);
			SDL_Delay(1000);
		}
	}

	imguiInit();

	return success;
}


bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	gOut = SDL_LoadBMP("images/TheFinalPowerPose.bmp");
	IMAGE_WIDTH = gOut->w / 2;
	IMAGE_HEIGHT = gOut->h / 2;

	gFont = TTF_OpenFont("fonts/WhiteRabbitTTF.ttf", 28);

	

	if (gOut == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", "images/TheFinalPowerPose.bmp", SDL_GetError());
		success = false;
	}

	garFeldiTexture = SDL_CreateTextureFromSurface(renderer, gOut);
	SDL_FreeSurface(gOut);


	return success;
}

void close()
{
	//Free loaded images

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window    
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();
}

void SetScreenColor(SDL_Color inputColor)
{
	SDL_SetRenderDrawColor(renderer, inputColor.r, inputColor.g, inputColor.b, inputColor.a);
	currentColor = inputColor;
}

bool SameColor(SDL_Color color1, SDL_Color color2)
{

	if (color1.r != color2.r) return false;
	if (color1.g != color2.g)  return false;
	if (color1.b != color2.b)  return false;
	if (color1.a != color2.a)  return false;

	return true;
}


Rectangle buttonRect{ (SCREEN_WIDTH / 2)-8, (SCREEN_HEIGHT / 2)-8, 16, 16 };

bool loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (textTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			textSurface->w = 25;
			textSurface->h = 25;
		}
		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return textTexture != NULL;
}


/*void OnButtonPress(void(*func)())
{
	func();
} */

void keepInBounds(int& slotX, int& slotY, const Grid &gameGrid)
{
	if (slotX >= gameGrid.rowSize) slotX--;
	if (slotX < 0) slotX++;
	if (slotY < 0) slotY++;
	if (slotY >= gameGrid.columnSize) slotY--;
}

bool isViableSpot(int slotX, int slotY, const Grid& gameGrid)
{
	if (slotX >= gameGrid.rowSize || slotX < 0)
	{
		cout << "NOPE, CANNOT DO IT BUDDY" << endl;
		return false;
	}
	if (slotY >= gameGrid.rowSize || slotY < 0)
	{
		cout << "NOPE, CANNOT DO IT BUDDY" << endl;
		return false;
	}

	if (gameGrid.grid.at(slotX).at(slotY).occupiedByLamd || gameGrid.grid.at(slotX).at(slotY).occupiedByPlayer)
	{
		cout << "Player in the way man." << endl;
		return false;
	}
	return true;
}

void DisplayCurrentCharges(int currentCharge, int maxCharge, bool currentlySelected = false)
{
	ImVec2 displayItemMin = ImGui::GetItemRectMax();
	ImVec2 displayItemMax = ImGui::GetItemRectMax();
	displayItemMax.x += 54;
	displayItemMin.y -= 20;

	float itemMin = displayItemMin.x += 20;
	float itemMax = displayItemMax.x;

	int chargesMade = currentCharge;

	for (int i = 0; i < maxCharge; i++)
	{
		if(chargesMade == 0 && currentlySelected) ImGui::GetWindowDrawList()->AddRectFilled(displayItemMin, displayItemMax, ImGui::GetColorU32({ 25 / 255.0f, 25 / 255.0f, 175 / 255.0f, 255 / 255.0f }), 0.5f);
		else if(chargesMade > 0) ImGui::GetWindowDrawList()->AddRectFilled(displayItemMin, displayItemMax, ImGui::GetColorU32({ 35 / 255.0f, 225 / 255.0f, 15 / 255.0f, 255 / 255.0f }), 0.5f);
		else ImGui::GetWindowDrawList()->AddRectFilled(displayItemMin, displayItemMax, ImGui::GetColorU32({ 125 / 255.0f, 125 / 255.0f, 125 / 255.0f, 255 / 255.0f }), 0.5f);
	//	else ImGui::GetWindowDrawList()->AddRectFilled(displayItemMin, displayItemMax, ImGui::GetColorU32({ 150 / 255.0f, 125 / 255.0f, 0 / 255.0f, 125 / 255.0f }), 2.f);
		displayItemMin.x += 48;
		displayItemMax.x += 48;

		chargesMade--;
	}
}


void checkForTriggeredMines()
{

}

void UseSonarItem()
{

}
#undef main
int main(int argc, char* args[])
{
	// variables

	bool quit = false;
	int gamestate = 0;
	Rectangle testRect(100, 100, 100, 100, {0, 95, 125});
	if (!init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}

	SDL_Init(SDL_INIT_VIDEO);

	if (!loadMedia())
	{
		printf("Failed to load media!\n");
		return 0;
	};

	srand(time(0));

	Grid gameGrid(16,16, SCREEN_WIDTH, SCREEN_HEIGHT);

	int x = gameGrid.GridSlotPosition(1,2).x;
	int y = gameGrid.GridSlotPosition(1,2).y;

	Player player1("Player 1", x, y, 24, 24);
	player1.playerNumber = 1;

	 x = gameGrid.GridSlotPosition(3, 4).x;
	 y = gameGrid.GridSlotPosition(3, 4).y;

 	Player player2("Player 2", x, y, 24, 24);

	player2.playerNumber = 2;


	player1.slotPosition = Vector2 { 0, 0 };
	player2.slotPosition = Vector2 { 4, 5 };


	Vector2 p1SlotPos = gameGrid.GridSlotPosition(player1.slotPosition.x, player1.slotPosition.y);
	Vector2 p2SlotPos = gameGrid.GridSlotPosition(player2.slotPosition.x, player2.slotPosition.y);

	player1.SetPosition(p1SlotPos.x, p1SlotPos.y);
	player2.SetPosition(p2SlotPos.x, p2SlotPos.y);

	Player currentPlayer = player1;

	SDL_Color baseColor{ 25, 65, 215, 255 };
	SDL_Color altColor{ 35, 187, 75, 255 };

	SDL_Color baseButtonColor{ 99, 15, 3, 255 };
	SDL_Color hoverButtonColor{ 135, 15, 3, 255 };

	SDL_Color currentButtonColor = baseButtonColor;

	SDL_Color textCOLOR{ 25, 25, 99, 255 };

	// init SDL

	// handle events

	SDL_Event e;

	SetScreenColor(baseColor);

	GameObject testOBJ;

	bool player1Turn = true;
	bool player2Turn = false;

	
	Vector2 previousPosition = currentPlayer.slotPosition;

	bool launchingTorpedo = false;

	std::string currentText = "It is " + player1.name + "'s Turn!";

	while (!quit)
	{
		if (gamestate == 0) {
			while (SDL_PollEvent(&e) != 0)
			{

			}

		}
		else if (gameState == 1){
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				ImGui_ImplSDL2_ProcessEvent(&e);

				if (e.type == SDL_MOUSEMOTION)
				{
					/*	if (inButtonBounds(e.motion.x, e.motion.y))
						{
							currentButtonColor = hoverButtonColor;
						}
						else currentButtonColor = baseButtonColor; */
				}

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					//if (!inButtonBounds(e.motion.x, e.motion.y)) break;

					if (SameColor(currentColor, baseColor))
					{
						SetScreenColor(altColor);
						//	loadFromRenderedText("Welcome to a New Scene!!!", textCOLOR);
					}
					else if (SameColor(currentColor, altColor))
					{
						SetScreenColor(baseColor);
					}

					break;
				}

				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				else if (e.type == SDL_KEYDOWN)
				{

					bool usedTurn = false;

					switch (e.key.keysym.sym)
					{

					case SDLK_RIGHT:
						if (isViableSpot(currentPlayer.slotPosition.x + 1, currentPlayer.slotPosition.y, gameGrid))
						{
							previousPosition = currentPlayer.slotPosition;
							currentPlayer.slotPosition.x += 1;
							usedTurn = true;
						}
						break;
					case SDLK_LEFT:
						if (isViableSpot(currentPlayer.slotPosition.x - 1, currentPlayer.slotPosition.y, gameGrid))
						{
							previousPosition = currentPlayer.slotPosition;
							currentPlayer.slotPosition.x -= 1;
							usedTurn = true;
						}
						break;
					case SDLK_UP:
						if (isViableSpot(currentPlayer.slotPosition.x, currentPlayer.slotPosition.y - 1, gameGrid))
						{
							previousPosition = currentPlayer.slotPosition;
							currentPlayer.slotPosition.y -= 1;
							usedTurn = true;
						}
						break;
					case SDLK_DOWN:
						if (isViableSpot(currentPlayer.slotPosition.x, currentPlayer.slotPosition.y + 1, gameGrid))
						{
							previousPosition = currentPlayer.slotPosition;
							currentPlayer.slotPosition.y += 1;
							usedTurn = true;
						}
						break;
					}

					keepInBounds(currentPlayer.slotPosition.x, currentPlayer.slotPosition.y, gameGrid);

					if (usedTurn)
					{
						if (currentPlayer.mineItem.isCharging)
						{
							currentPlayer.mineItem.isCharging = false;
							currentPlayer.mineItem.currentCharge++;
						}
						if (currentPlayer.sonarItem.isCharging)
						{
							currentPlayer.sonarItem.isCharging = false;
							currentPlayer.sonarItem.currentCharge++;
						}
						gameGrid.grid.at(previousPosition.x).at(previousPosition.y).occupiedByPlayer = false;
						gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).occupiedByPlayer = true;
					}


					if (player1Turn && usedTurn)
					{
						player1 = currentPlayer;
						currentPlayer = player2;
						player1Turn = false;

						currentText = "It is " + currentPlayer.name + "'s Turn!";
					}
					else if (usedTurn)
					{
						player2 = currentPlayer;
						currentPlayer = player1;
						player1Turn = true;

						currentText = "It is " + currentPlayer.name + "'s Turn!";
					}

					Vector2 p1SlotPos = gameGrid.GridSlotPosition(player1.slotPosition.x, player1.slotPosition.y);
					Vector2 p2SlotPos = gameGrid.GridSlotPosition(player2.slotPosition.x, player2.slotPosition.y);

					player1.SetPosition(p1SlotPos.x, p1SlotPos.y);
					player2.SetPosition(p2SlotPos.x, p2SlotPos.y);
				}
				else if (e.type == SDL_KEYUP)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_RIGHT:
						break;
					}
				}

				/*			if (x > SCREEN_WIDTH - (IMAGE_WIDTH)) x -= 1;
							else if (x < 0) x += 1;
							if (y > SCREEN_HEIGHT-(IMAGE_HEIGHT)) y -= 1;
							else if (y < 0) y += 1; */
			}

			ImGui_ImplSDLRenderer2_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			//imguiFrame();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.173f, 0.427f, 0.702f, 1.0f)); // Set window background to red
			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0.435, 0.902, 0.85f));
			ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.004f, 0.243f, 0.502f, 0.85f));
			ImGui::Begin("Take Your Turn!");

			if (ImGui::Button("Mine"))
			{
				if (currentPlayer.mineItem.currentCharge == currentPlayer.mineItem.requiredCharge)
				{
					currentPlayer.mineItem.currentCharge = 0;
					gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).occupiedByMine = true;

				}
				else
				{
					if (!currentPlayer.mineItem.isCharging) currentPlayer.mineItem.isCharging = true;
					else currentPlayer.mineItem.isCharging = false;
				}
				// Buttons return true when clicked (most widgets return true when edited/activated)
			}
			DisplayCurrentCharges(currentPlayer.mineItem.currentCharge, currentPlayer.mineItem.requiredCharge, currentPlayer.mineItem.isCharging);
			if (ImGui::Button("Drone"))
			{
				// Buttons return true when clicked (most widgets return true when edited/activated)
			//	if (currentPlayer.torpedoItem.currentCharge == currentPlayer.torpedoItem.requiredCharge)
				{
					//		launchingTorpedo = true;
				}

				if (gameGrid.CheckSectorForPlayer(4)) cout << "PLAYER IS IN THE SECTOR" << endl;
				else cout << "PLAYER IS NOT IN THE SECTOR" << endl;
			}
			DisplayCurrentCharges(currentPlayer.droneItem.currentCharge, currentPlayer.droneItem.requiredCharge, currentPlayer.droneItem.isCharging);
			if (ImGui::Button("Torpedo"))
			{
				currentText = "Input offset from current position to place your torpedo!";
				int x;
				int y;

				// Buttons return true when clicked (most widgets return true when edited/activated)
				gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).occupiedByMine = true;
			}
			DisplayCurrentCharges(currentPlayer.torpedoItem.currentCharge, currentPlayer.torpedoItem.requiredCharge, currentPlayer.torpedoItem.isCharging);
			if (ImGui::Button("Sonar"))
			{
				if (currentPlayer.sonarItem.currentCharge == currentPlayer.sonarItem.requiredCharge)
				{
					currentPlayer.sonarItem.currentCharge = 0;

					int randomRow = rand() % gameGrid.rowSize;
					int randomColum = rand() % gameGrid.columnSize;

					bool chance = rand() % 2;

					if (chance)
					{
						if (player1Turn) cout << "Opponent Location: X: " << randomRow << " Y: " << player2.slotPosition.y << endl;
						else  cout << "Opponent Location: X: " << randomRow << " Y: " << player1.slotPosition.y << endl;
					}
					else
					{
						if (player1Turn) cout << "Opponent Location: X: " << player2.slotPosition.x << " Y: " << randomColum << endl;
						else  cout << "Opponent Location: X: " << player1.slotPosition.x << " Y: " << randomColum << endl;
					}

				}
				else
				{
					if (!currentPlayer.sonarItem.isCharging) currentPlayer.sonarItem.isCharging = true;
					else currentPlayer.sonarItem.isCharging = false;
				}
			}
			DisplayCurrentCharges(currentPlayer.sonarItem.currentCharge, currentPlayer.sonarItem.requiredCharge, currentPlayer.sonarItem.isCharging);

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::End();

			SDL_Rect textRect = { 24, 24, 300, 114 };


			ImGui::Render();

			loadFromRenderedText(currentText, textCOLOR);

			SDL_RenderClear(renderer);


			//testRect.Render(renderer);

			gameGrid.RenderGrid(renderer);

			//Vector2 player1Pos = gameGrid.GridSlotPosition(player1SlotPos.x, player1SlotPos.y);
			//Vector2 player2Pos = gameGrid.GridSlotPosition(player2SlotPos.x, player2SlotPos.y);

			if (player1Turn)
			{
				player1.Render(renderer, true);
				player2.Render(renderer, false);
			}
			else
			{
				player1.Render(renderer, false);
				player2.Render(renderer, true);
			}

			ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
			//SDL_RenderCopy(renderer, garFeldiTexture, NULL, &dstrect);

			buttonRect.Render(renderer);
			SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
			SDL_RenderPresent(renderer);

			SetScreenColor({ 51, 155, 226 , 225 });
			//loadFromRenderedText("Welcome to a New Scene!!!", textCOLOR);
			SDL_RenderClear(renderer);
		}

	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// cleanup SDL
	SDL_DestroyTexture(garFeldiTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}

