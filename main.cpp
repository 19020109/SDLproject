#include <iostream>
#include <string>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "gameScr.hpp"

using namespace std;

const string WINDOW_TITLE = "Pooong";
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float PADDLE_SPEED = 1.0f;
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 100;
const float BALL_SPEED = 1.0f;
const int BALL_WIDTH = 15;
const int BALL_HEIGHT = 15;

enum Buttons{
    PaddleOneUp,
    PaddleOneDown,
    PaddleTwoUp,
    PaddleTwoDown,
};

enum class CollisionType{
    None,
    Top,
    Middle,
    Bottom,
    Left,
    Right
};

struct Contact{
    CollisionType type;
    float penetration;
};

class Vec2{
public:
    Vec2()
        : x(0.0f), y(0.0f)
    {}

    Vec2(float x, float y)
        : x(x), y(y)
    {}

    Vec2 operator+(Vec2 const& rhs){
        return Vec2(x + rhs.x, y + rhs.y);
    }

    Vec2& operator+=(Vec2 const& rhs){
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator*(float rhs){
        return Vec2(x * rhs, y * rhs);
    }

    float x, y;
};

class Paddle{
public:
    Paddle(Vec2 position, Vec2 velocity)
        : position(position), velocity(velocity){
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);
        rect.w = PADDLE_WIDTH;
        rect.h = PADDLE_HEIGHT;
    }

    void Update(float dt){
        position += velocity * dt;

        if (position.y < 0){
            position.y = 0;
        }
        else if (position.y > (SCREEN_HEIGHT - PADDLE_HEIGHT)){
            position.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
        }
    }

    void Draw(SDL_Renderer* renderer){
        rect.y = static_cast<int>(position.y);

        SDL_RenderFillRect(renderer, &rect);
    }

    Vec2 position;
    Vec2 velocity;
    SDL_Rect rect{};
};


class Ball{
public:
    Ball(Vec2 position, Vec2 velocity)
        : position(position), velocity(velocity){
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);
        rect.w = BALL_WIDTH;
        rect.h = BALL_HEIGHT;
    }

    void Update(float dt){
        position += velocity * dt;
    }

    void Draw(SDL_Renderer* renderer){
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);

        SDL_RenderFillRect(renderer, &rect);
    }

    void CollideWithPaddle(Contact const& contact){
        position.x += contact.penetration;
        velocity.x = -velocity.x;

        if (contact.type == CollisionType::Top){
            velocity.y = -.75f * BALL_SPEED;
        }
        else if (contact.type == CollisionType::Bottom){
            velocity.y = 0.75f * BALL_SPEED;
        }
    }

    void CollideWithWall(Contact const& contact){
        if ((contact.type == CollisionType::Top)
            || (contact.type == CollisionType::Bottom)){
            position.y += contact.penetration;
            velocity.y = -velocity.y;
        }
        else if (contact.type == CollisionType::Left){
            position.x = SCREEN_WIDTH / 2;
            position.y = SCREEN_HEIGHT / 2;
            velocity.x = BALL_SPEED;
            velocity.y = 0.75f * BALL_SPEED;
        }
        else if (contact.type == CollisionType::Right){
            position.x = SCREEN_WIDTH / 2;
            position.y = SCREEN_HEIGHT / 2;
            velocity.x = -BALL_SPEED;
            velocity.y = 0.75f * BALL_SPEED;
        }
    }

    Vec2 position;
    Vec2 velocity;
    SDL_Rect rect{};
};



Contact CheckPaddleCollision(Ball const& ball, Paddle const& paddle){
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    float paddleLeft = paddle.position.x;
    float paddleRight = paddle.position.x + PADDLE_WIDTH;
    float paddleTop = paddle.position.y;
    float paddleBottom = paddle.position.y + PADDLE_HEIGHT;

    Contact contact{};

    if (ballLeft >= paddleRight){
        return contact;
    }

    if (ballRight <= paddleLeft){
        return contact;
    }

    if (ballTop >= paddleBottom){
        return contact;
    }

    if (ballBottom <= paddleTop){
        return contact;
    }

    float paddleRangeUpper = paddleBottom - (2 * PADDLE_HEIGHT / 3);
    float paddleRangeMiddle = paddleBottom - (PADDLE_HEIGHT / 3);

    if (ball.velocity.x < 0){
        contact.penetration = paddleRight - ballLeft;
    }
    else if (ball.velocity.x > 0){
        contact.penetration = paddleLeft - ballRight;
    }

    if ((ballBottom > paddleTop)
        && (ballBottom < paddleRangeUpper)){
        contact.type = CollisionType::Top;
    }
    else if ((ballBottom > paddleRangeUpper)
             && (ballBottom < paddleRangeMiddle)){
        contact.type = CollisionType::Middle;
    }
    else{
        contact.type = CollisionType::Bottom;
    }

    return contact;
}


Contact CheckWallCollision(Ball const& ball)
{
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    Contact contact{};

    if (ballLeft < 0){
        contact.type = CollisionType::Left;
    }
    else if (ballRight > SCREEN_WIDTH){
        contact.type = CollisionType::Right;
    }
    else if (ballTop < 0){
        contact.type = CollisionType::Top;
        contact.penetration = -ballTop;
    }
    else if (ballBottom > SCREEN_HEIGHT){
        contact.type = CollisionType::Bottom;
        contact.penetration = SCREEN_HEIGHT - ballBottom;
    }

    return contact;
}


int main(){
    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

    {
        Ball ball(
            Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2),
            Vec2(BALL_SPEED, 0.0f));

        Paddle paddleOne(
            Vec2(50.0f, SCREEN_HEIGHT / 2),
            Vec2(0.0f, 0.0f));

        Paddle paddleTwo(
            Vec2(SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT / 2),
            Vec2(0.0f, 0.0f));

        bool running = true;
        bool buttons[4] = {};

        float dt = 0;

        while (running){
            
            auto startTime = std::chrono::high_resolution_clock::now();

            SDL_Event event;
            while (SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT){
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN){

                    if (event.key.keysym.sym == SDLK_ESCAPE){
                        running = false;
                    }
                    else if (event.key.keysym.sym == SDLK_w){
                        buttons[Buttons::PaddleOneUp] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_s){
                        buttons[Buttons::PaddleOneDown] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_UP){
                        buttons[Buttons::PaddleTwoUp] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN){
                        buttons[Buttons::PaddleTwoDown] = true;
                    }
                }
                else if (event.type == SDL_KEYUP){
                    
                    if (event.key.keysym.sym == SDLK_w){
                        buttons[Buttons::PaddleOneUp] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_s){
                        buttons[Buttons::PaddleOneDown] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_UP){
                        buttons[Buttons::PaddleTwoUp] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN){
                        buttons[Buttons::PaddleTwoDown] = false;
                    }
                }
            }

            if (buttons[Buttons::PaddleOneUp]){
                paddleOne.velocity.y = -PADDLE_SPEED;
            }
            else if (buttons[Buttons::PaddleOneDown]){
                paddleOne.velocity.y = PADDLE_SPEED;
            }
            else{
                paddleOne.velocity.y = 0;
            }

            if (buttons[Buttons::PaddleTwoUp]){
                paddleTwo.velocity.y = -PADDLE_SPEED;
            }
            else if (buttons[Buttons::PaddleTwoDown]){
                paddleTwo.velocity.y = PADDLE_SPEED;
            }
            else{
                paddleTwo.velocity.y = 0;
            }

            paddleOne.Update(dt);
            paddleTwo.Update(dt);

            ball.Update(dt);

            if (Contact contact = CheckPaddleCollision(ball, paddleOne);
                contact.type != CollisionType::None){
                ball.CollideWithPaddle(contact);
            }
            else if (contact = CheckPaddleCollision(ball, paddleTwo);
                contact.type != CollisionType::None){
                ball.CollideWithPaddle(contact);
            }
            else if (contact = CheckWallCollision(ball);
                contact.type != CollisionType::None){
                ball.CollideWithWall(contact);
            }

            SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            for (int y = 0; y < SCREEN_HEIGHT; ++y){
                if (y % 5){
                    SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, y);
                }
            }

            ball.Draw(renderer);
            paddleOne.Draw(renderer);
            paddleTwo.Draw(renderer);

            SDL_RenderPresent(renderer);

            auto stopTime = std::chrono::high_resolution_clock::now();
            dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
        }
    }
    
    quitSDL(window, renderer);
    return 0;
}
