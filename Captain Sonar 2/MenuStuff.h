#include <string>
#include <memory>
#include <SDL_events.h>
#include <SDL.h>
#include <SDL_ttf.h>

using namespace std;

class Button {

	protected:

		SDL_Color idleColor;
		SDL_Color hoverColor;
		SDL_Color clickColor;

		string textChosen;

	public:
		
		SDL_Rect shape;
		SDL_Color color;

		int horizontalPos;
		int verticalPos;

		int buttonID; // Thing says differentiate button Types;

		Button(float xPosStart, float yPosStart, float width, float height, SDL_Color newIdleColor, SDL_Color newHoverColor, SDL_Color newClickColor, string newText);
		

		~Button();

};
