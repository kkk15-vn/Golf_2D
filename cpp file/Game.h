#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "Ball.h"
#include "Obstacle.h"
#include "Menu.h"

class Game {
public:
    Game();
    ~Game();
    bool init(const char* title, int width, int height);
    void run();
    void cleanup();
private:
    void handleEvents();
    void update();
    void render();
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Ball ball;
    std::vector<Obstacle> obstacles;
    Menu menu;
};

#endif
