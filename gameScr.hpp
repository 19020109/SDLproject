#ifndef gameon_hpp
#define gameon_hpp

#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

using namespace std;

void logSDLError(std::ostream& os, const std::string &msg, bool fatal);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer,
             int SCREEN_WIDTH,int SCREEN_HEIGHT, const string &WINDOW_TITLE);

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
#endif 
