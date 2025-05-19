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

#include "Client.h"
#include "Server.h"
#undef main
using namespace std;

extern const int SCREEN_WIDTH = 1280;
extern const int SCREEN_HEIGHT = 720;

int IMAGE_WIDTH = 25;
int IMAGE_HEIGHT = 25;

Level menuLevel("MainMenu", 0, 100, 85);
Level gameLevel("Game Scene", 50, 150, 100);

std::string currentText = "It is Player 1's Turn!";

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window

//The image we will load and show on the screen
SDL_Surface* gOut = NULL;

SDL_Renderer* renderer = NULL;

SDL_Texture* sonarIconTexture = NULL;
SDL_Texture* torpedoIconTexture = NULL;
SDL_Texture* droneIconTexture = NULL;
SDL_Texture* mineIconTexture = NULL;
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
	cout << "Begging, Begging you to work buddy." << endl;
	//Initialize SDL
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
	gOut = SDL_LoadBMP("images/SonarIcon.bmp");
	IMAGE_WIDTH = gOut->w / 2;
	IMAGE_HEIGHT = gOut->h / 2;
	sonarIconTexture = SDL_CreateTextureFromSurface(renderer, gOut);
	SDL_FreeSurface(gOut);

	gOut = SDL_LoadBMP("images/TorpedoIcon.bmp");
	IMAGE_WIDTH = gOut->w / 2;
	IMAGE_HEIGHT = gOut->h / 2;
	torpedoIconTexture = SDL_CreateTextureFromSurface(renderer, gOut);
	SDL_FreeSurface(gOut);

	gOut = SDL_LoadBMP("images/DroneIcon.bmp");
	IMAGE_WIDTH = gOut->w / 2;
	IMAGE_HEIGHT = gOut->h / 2;
	droneIconTexture = SDL_CreateTextureFromSurface(renderer, gOut);
	SDL_FreeSurface(gOut);

	gOut = SDL_LoadBMP("images/MineIcon.bmp");
	IMAGE_WIDTH = gOut->w / 2;
	IMAGE_HEIGHT = gOut->h / 2;
	mineIconTexture = SDL_CreateTextureFromSurface(renderer, gOut);
	SDL_FreeSurface(gOut);

	gFont = TTF_OpenFont("fonts/WhiteRabbitTTF.ttf", 28);


	SDL_FreeSurface(gOut);

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


Rectangle1 buttonRect{ (SCREEN_WIDTH / 2) - 8, (SCREEN_HEIGHT / 2) - 8, 16, 16 };

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

void keepInBounds(int& slotX, int& slotY, const Grid& gameGrid)
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

	if (gameGrid.grid.at(slotX).at(slotY).occupiedByLand || gameGrid.grid.at(slotX).at(slotY).occupiedByPlayer)
	{
		cout << "Player in the way man." << endl;
		return false;
void GenerateLandmass(Grid& gameGrid)
{
	gameGrid.grid.at(3).at(3).occupiedByLand = true;
	gameGrid.grid.at(3).at(4).occupiedByLand = true;
	gameGrid.grid.at(3).at(5).occupiedByLand = true;
	gameGrid.grid.at(2).at(3).occupiedByLand = true;

	gameGrid.grid.at(10).at(7).occupiedByLand = true;
	gameGrid.grid.at(11).at(7).occupiedByLand = true;
	gameGrid.grid.at(12).at(7).occupiedByLand = true;
	gameGrid.grid.at(11).at(8).occupiedByLand = true;
	gameGrid.grid.at(11).at(9).occupiedByLand = true;

	gameGrid.grid.at(3).at(9).occupiedByLand = true;
	gameGrid.grid.at(3).at(10).occupiedByLand = true;
	gameGrid.grid.at(3).at(11).occupiedByLand = true;
	gameGrid.grid.at(4).at(12).occupiedByLand = true;
	gameGrid.grid.at(5).at(12).occupiedByLand = true;
	gameGrid.grid.at(6).at(13).occupiedByLand = true;
	gameGrid.grid.at(7).at(13).occupiedByLand = true;

	gameGrid.grid.at(12).at(12).occupiedByLand = true;
	gameGrid.grid.at(13).at(12).occupiedByLand = true;
	gameGrid.grid.at(12).at(13).occupiedByLand = true;
	gameGrid.grid.at(11).at(13).occupiedByLand = true;
	gameGrid.grid.at(10).at(13).occupiedByLand = true;
}

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
		if (chargesMade == 0 && currentlySelected) ImGui::GetWindowDrawList()->AddRectFilled(displayItemMin, displayItemMax, ImGui::GetColorU32({ 25 / 255.0f, 25 / 255.0f, 175 / 255.0f, 255 / 255.0f }), 0.5f);
		else if (chargesMade > 0) ImGui::GetWindowDrawList()->AddRectFilled(displayItemMin, displayItemMax, ImGui::GetColorU32({ 35 / 255.0f, 225 / 255.0f, 15 / 255.0f, 255 / 255.0f }), 0.5f);
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

bool takingDamage = false;
bool endingGame = false;

void DamagePlayer(Player& player, int damage)
{
	player.currentHealth -= damage;
	takingDamage = true;
	if (player.currentHealth == 0) endingGame = true;
}

void TorpedoExplosion(int slotX, int slotY, Grid& gameGrid, Player& p1, Player& p2, Player& currentPlayer)
{
	int minX = slotX - 1;
	int minY = slotY - 1;
	int maxX = slotX + 1;
	for (int i = minX; i <= maxX; i++)
	{
		for (int j = minY; j <= maxY; j++)
		{
			if (i >= gameGrid.rowSize || i < 0)
			{
				continue;
			}
			if (j >= gameGrid.columnSize || j < 0)
			{
				continue;
			}

			if (gameGrid.grid.at(i).at(j).occupiedByPlayer)
			{
				if (p1.slotPosition.x == i && p1.slotPosition.y == j)
				{
					cout << "PLAYER HIT!" << endl;
					DamagePlayer(p1, 1);
					hitSomething = true;
				}
				if (p2.slotPosition.x == i && p2.slotPosition.y == j)
				{
					cout << "PLAYER HIT!" << endl;
					DamagePlayer(p2, 1);
					hitSomething = true;
				}
				if (currentPlayer.slotPosition.x == i && currentPlayer.slotPosition.y == j)
				{

				}
			}
		}
	}

	if (!hitSomething) cout << "Nothing was Hit!" << endl;
}

	bool hitSomething = false;

}
#undef main
int main(int argc, char* args[])
{
	// variables

	Rectangle1 testRect(100, 100, 100, 100, { 0, 95, 125 });

	int textDisplayTime = 1;


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
	}

	srand(time(0));

	Grid gameGrid(16, 16, SCREEN_WIDTH, SCREEN_HEIGHT);

	int x = gameGrid.GridSlotPosition(1, 2).x;
	int y = gameGrid.GridSlotPosition(1, 2).y;

	player1 = Player("Player 1", x, y, 24, 24);
	player1.playerNumber = 1;

	x = gameGrid.GridSlotPosition(3, 4).x;
	y = gameGrid.GridSlotPosition(3, 4).y;

	player2 = Player("Player 2", x, y, 24, 24);
	player2.playerNumber = 2;


	player1.slotPosition = Vector2{ 0, 0 };
	player2.slotPosition = Vector2{ 4, 5 };


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

	GenerateLandmass(gameGrid);

	// init SDL

	// handle events

	int xInput = 1;

	int yInput = 1;

	int sectorInput = 1;

	SDL_Event e;

	SetScreenColor(baseColor);

	GameObject testOBJ;

	bool player1Turn = true;
	bool player2Turn = false;


	bool inMainMenu = true;
	static char ipBuffer[64] = "127.0.0.1";
	static char portBuffer[16] = "8080";
	while (inMainMenu)

	std::string currentText = "It is " + player1.name + "'s Turn!";

	while (!quit)
	{

		while (SDL_PollEvent(&e) != 0)
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT)
			{
				inMainMenu = false;

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//imguiFrame();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.173f, 0.427f, 0.702f, 1.0f)); // Set window background to red
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0.435, 0.902, 0.85f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.004f, 0.243f, 0.502f, 0.85f));

		ImGui::Begin("Main Menu Bud");

		if (ImGui::Button("Play Game")) {
			readyup = !readyup;
			//inMainMenu = false;
				quit = true;
		if (startGame) inMainMenu = false;
		ImGui::Spacing();
		ImGui::InputText("IP Address", ipBuffer, sizeof(ipBuffer));
		ImGui::InputText("Port", portBuffer, sizeof(portBuffer));
		if (ImGui::Button("Connect to Server")) {
			std::string ip = ipBuffer;
			int port = std::stoi(portBuffer);
			// Your connect logic here
			connect(ip, to_string(port));
		}

		ImGui::InputText("Port1", portBuffer, sizeof(portBuffer));
		if (ImGui::Button("Start Server")) {
			int port = std::stoi(portBuffer);
			startServer(port);
		}



		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::End();


		ImGui::Render();

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

		SDL_RenderPresent(renderer);

		SetScreenColor({ 51, 155, 226 , 225 });
		//loadFromRenderedText("Welcome to a New Scene!!!", textCOLOR);
		SDL_RenderClear(renderer);
	}


	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			ImGui_ImplSDL2_ProcessEvent(&e);
			while (SDL_PollEvent(&e) != 0)
			if (e.type == SDL_MOUSEMOTION)
			{
				/*	if (inButtonBounds(e.motion.x, e.motion.y))
					{
						currentButtonColor = hoverButtonColor;
					}
					else currentButtonColor = baseButtonColor; */
			}
							currentButtonColor = hoverButtonColor;
						}
						else currentButtonColor = baseButtonColor; */
				}

				if (SameColor(currentColor, baseColor))
				{
					SetScreenColor(altColor);
					//	loadFromRenderedText("Welcome to a New Scene!!!", textCOLOR);
				}
				else if (SameColor(currentColor, altColor))
				{
					SetScreenColor(baseColor);
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
					gameGrid.grid.at(previousPosition.x).at(previousPosition.y).playerOwner = 0;

					gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).occupiedByPlayer = true;
					gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).playerOwner = currentPlayer.playerNumber;

					if (gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).occupiedByMine)
					{
						DamagePlayer(currentPlayer, 1);
						gameGrid.grid.at(currentPlayer.slotPosition.x).at(currentPlayer.slotPosition.y).occupiedByMine = false;
						textDisplayTime = SDL_GetTicks() + 1500;
				if (isTurn)
				{
					player1 = currentPlayer;
					currentPlayer = player2;
					player1Turn = false;

					currentText = "It is Player 1's Turn!";
				}
				else if (!isTurn)
				{
					player2 = currentPlayer;
					currentPlayer = player1;
					player1Turn = true;
					{
					currentText = "It is Player 2's Turn!";
				}

				if (takingDamage && textDisplayTime > SDL_GetTicks())
				{
					currentText = "Taken Damage!!!";
				}
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
			/*			if (x > SCREEN_WIDTH - (IMAGE_WIDTH)) x -= 1;
						else if (x < 0) x += 1;
						if (y > SCREEN_HEIGHT-(IMAGE_HEIGHT)) y -= 1;
						else if (y < 0) y += 1; */
		}
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

		if (player1Turn) ImGui::Text("Player1 Info");
		else ImGui::Text("Player2 Info");

		ImGui::Spacing();
		ImGui::Spacing();


		ImGui::Text("Health");
		DisplayCurrentCharges(currentPlayer.currentHealth, currentPlayer.maxHealth, false);

		ImGui::Image((ImTextureID)mineIconTexture, ImVec2(34, 34));
		ImGui::SameLine();
		if (ImGui::Button("Mine"))
		{
			if (currentPlayer.mineItem.currentCharge == currentPlayer.mineItem.requiredCharge)
			}
			else
			{
				if (!currentPlayer.mineItem.isCharging) currentPlayer.mineItem.isCharging = true;
				else currentPlayer.mineItem.isCharging = false;
			}
			// Buttons return true when clicked (most widgets return true when edited/activated)
		}
		DisplayCurrentCharges(currentPlayer.mineItem.currentCharge, currentPlayer.mineItem.requiredCharge, currentPlayer.mineItem.isCharging);

		ImGui::Image((ImTextureID)droneIconTexture, ImVec2(34, 34));
		ImGui::SameLine();

		if (ImGui::Button("Drone"))
		{
			// Buttons return true when clicked (most widgets return true when edited/activated)
		//	if (currentPlayer.torpedoItem.currentCharge == currentPlayer.torpedoItem.requiredCharge)
			{
				//		launchingTorpedo = true;
			}

			if (gameGrid.CheckSectorForPlayer(sectorInput)) cout << "PLAYER IS IN THE SECTOR" << endl;
			else cout << "PLAYER IS NOT IN THE SECTOR" << endl;
		}
		DisplayCurrentCharges(currentPlayer.droneItem.currentCharge, currentPlayer.droneItem.requiredCharge, currentPlayer.droneItem.isCharging);
		ImGui::Text("Sector To Scan");
		ImGui::SliderInt("##sectorinput", &sectorInput, 1, 4);

		ImGui::Image((ImTextureID)torpedoIconTexture, ImVec2(34, 34));
		ImGui::SameLine();

		if (ImGui::Button("Torpedo"))
		{
			// Buttons return true when clicked (most widgets return true when edited/activated)
			TorpedoExplosion(currentPlayer.slotPosition.x + xInput, currentPlayer.slotPosition.y + -yInput, gameGrid, player1, player2, currentPlayer);
		}
		DisplayCurrentCharges(currentPlayer.torpedoItem.currentCharge, currentPlayer.torpedoItem.requiredCharge, currentPlayer.torpedoItem.isCharging);

		ImGui::Text("Torpedo X");
		ImGui::SliderInt("##xinput", &xInput, -2, 2);
		ImGui::Text("Torpedo Y");
		ImGui::SliderInt("##yinput", &yInput, -2, 2);

		ImGui::Image((ImTextureID)sonarIconTexture, ImVec2(34, 34));
		ImGui::SameLine();
		//ImGui::SetCursorPosY(ImGui::GetItemRectMin().y - 200);

		if (ImGui::Button("Sonar"))
		{
			if (currentPlayer.sonarItem.currentCharge == currentPlayer.sonarItem.requiredCharge)
			{
				currentPlayer.sonarItem.currentCharge = 0;
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
			}
			else
			{
				if (!currentPlayer.sonarItem.isCharging) currentPlayer.sonarItem.isCharging = true;
				else currentPlayer.sonarItem.isCharging = false;
			}
		}

		DisplayCurrentCharges(currentPlayer.sonarItem.currentCharge, currentPlayer.sonarItem.requiredCharge, currentPlayer.sonarItem.isCharging);
		if (ImGui::Button("Ready"))
		{
			ready = true;
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
			ImGui::PopStyleColor();
		SDL_Rect textRect = { 24, 24, 300, 114 };
		SDL_Rect iconRect = { 24, 90, 125, 125 };

			ImGui::End();

			SDL_Rect textRect = { 24, 24, 300, 114 };


		ImGui::Render();

		if (endingGame && textDisplayTime > SDL_GetTicks())
		{
			string victoryText;
			if (player1.currentHealth == 0) victoryText = "Player 2 Winner!";
			else victoryText = "Player 1 Winner!";
			currentText = victoryText;
			textDisplayTime--;
		}
		else if (endingGame && textDisplayTime <= SDL_GetTicks())
		{
			endingGame = false;
			quit = true;
		}

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
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
		//	SDL_RenderCopy(renderer, garFeldiTexture, NULL, &iconRect);
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

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// cleanup SDL
	SDL_DestroyTexture(sonarIconTexture);
	SDL_DestroyTexture(torpedoIconTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}
