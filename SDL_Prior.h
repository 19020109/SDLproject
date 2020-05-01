#ifndef SDL_Priority
#define SDL_Priority

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>

using namespace std;

void logSDLError(std::ostream& os, const std::string &msg, bool fatal);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer,
             int SCREEN_WIDTH,int SCREEN_HEIGHT, const string &WINDOW_TITLE);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void waitUntilKeyPressed();

void renderPlayer(SDL_Renderer* &renderer, SDL_Rect &player, int x, int y, int scale, vector<int> tailX, vector<int> tailY, int tailLength);

void renderFood(SDL_Renderer* &renderer, SDL_Rect &food);

#endif
