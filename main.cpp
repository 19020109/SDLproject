#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <vector>
#include "SDL_Prior.h"

using namespace std;
const string WINDOW_TITLE = "Snake";
const int SCREEN_WIDTH = 25*30+1;
const int SCREEN_HEIGHT = 25*25+1;

bool checkCollision(int foodX, int foodY, int playerX, int playerY) {
    if (playerX == foodX && playerY == foodY){
        return true;
    }
    return false;
}

pair<int, int> getFoodSpawn(vector<int> tailX, vector<int> tailY, int playerX, int playerY, int scale, int wScale, int tailLength) {
    bool valid = true;
    int x = 0, y = 0;
    srand(time(0));
    x = scale * (rand() % wScale);
    y = scale * (rand() % scale);

    for (int i = 0; i < tailLength; i++) {
        if ((x == tailX[i] && y == tailY[i]) || (x == playerX && y == playerY)) {
            valid = false;
        }
    }
    if (!valid) {
        pair<int, int> foodLoc;
        foodLoc = make_pair(-100, -100);
        return foodLoc;
    }
    pair<int, int> foodLoc;
    foodLoc = make_pair(x, y);
    return foodLoc;
}

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;

    SDL_Rect player;
    player.x = 0;
    player.y = 0;
    player.h = 0;
    player.w = 0;

    int tailLength = 0;

    vector<int> tailX;
    vector<int> tailY;

    int scale = 25, wScale = 30;

    int x = 0, y = 0;
    int prevX = 0, prevY = 0;

    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;

    bool inputThisFrame = false;
    bool redo = false;

    SDL_Rect food;
    food.w = scale;
    food.h = scale;
    food.x = 0;
    food.y = 0;
    
    pair<int, int> foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
    food.x = foodLoc.first;
    food.y = foodLoc.second;

    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

    float time = SDL_GetTicks() / 100;

    while (true) {

        float newTime = SDL_GetTicks() / 75;
        float delta = newTime - time;
        time = newTime;

        inputThisFrame = false;

        if (tailLength >= 25*30) {
            x = 0;
            y = 0;
            up = false;
            left = false;
            right = false;
            down = false;
            tailX.clear();
            tailY.clear();
            tailLength = 0;
            redo = false;
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }

            food.x = foodLoc.first;
            food.y = foodLoc.second;
        }

        if (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_KEYDOWN && inputThisFrame == false) {
                if (down == false && event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    up = true;
                    left = false;
                    right = false;
                    down = false;
                    inputThisFrame = true;
                }
                else if (right == false && event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                    up = false;
                    left = true;
                    right = false;
                    down = false;
                    inputThisFrame = true;
                }
                else if (up == false && event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    up = false;
                    left = false;
                    right = false;
                    down = true;
                    inputThisFrame = true;
                }
                else if (left == false && event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                    up = false;
                    left = false;
                    right = true;
                    down = false;
                    inputThisFrame = true;
                }

            }

        }

        prevX = x;
        prevY = y;

        if (up) {
            y -= delta * scale;
        }
        else if (left) {
            x -= delta * scale;
        }
        else if (right) {
            x += delta * scale;
        }
        else if (down) {
            y += delta * scale;
        }

        if (redo == true) {
            redo = false;
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
            food.x = foodLoc.first;
            food.y = foodLoc.second;

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }

        }

        if (checkCollision(food.x, food.y, x, y)) {
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
            food.x = foodLoc.first;
            food.y = foodLoc.second;

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }
            tailLength++;
        }

        if (delta * scale == 25) {
            if (tailX.size() != tailLength) {
                tailX.push_back(prevX);
                tailY.push_back(prevY);
            }
            for (int i = 0; i < tailLength; i++) {
                if (i == 0) continue;
                    tailX[i - 1] = tailX[i];
                    tailY[i - 1] = tailY[i];
            }
            if (tailLength > 0) {
                tailX[tailLength - 1] = prevX;
                tailY[tailLength - 1] = prevY;
            }
        }
        
        for (int i = 0; i < tailLength; i++) {
            if (x == tailX[i] && y == tailY[i]) {
                x = 0;
                y = 0;
                up = false;
                left = false;
                right = false;
                down = false;
                tailX.clear();
                tailY.clear();
                tailLength = 0;
                redo = false;

                foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
                if (food.x == -100 && food.y == -100) {
                    redo = true;
                }

                food.x = foodLoc.first;
                food.y = foodLoc.second;
            }

        }

        if (x < 0 || y < 0 || x > scale * wScale - scale || y > scale * scale - scale) {
            x = 0;
            y = 0;
            up = false;
            left = false;
            right = false;
            down = false;
            tailX.clear();
            tailY.clear();
            tailLength = 0;
            redo = false;
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
            food.x = foodLoc.first;
            food.y = foodLoc.second;

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }

        }
        renderFood(renderer, food);
        renderPlayer(renderer, player, x, y, scale, tailX, tailY, tailLength);
        
        SDL_RenderDrawLine(renderer, 0, 0, 0, 25*25);
        SDL_RenderDrawLine(renderer, 0, 0, 25*30, 0);
        SDL_RenderDrawLine(renderer, 25*30, 0, 25*30, 25*25);
        SDL_RenderDrawLine(renderer, 0, 25*25, 25*30, 25*25);
        
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
        SDL_RenderClear(renderer);
    }
    quitSDL(window, renderer);
    return 0;
}
