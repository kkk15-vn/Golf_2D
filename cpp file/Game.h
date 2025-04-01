#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "Ball.h"
#include "Hole.h"
#include "Obstacle.h"
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Ball ball;
    Hole hole;
    std::vector<Obstacle> obstacles;// Mảng 3 chướng ngại vật
    int currentLevel;
    int mouseClickCount = 0;
    int remainingTime;
    int maxTime;
    Uint32 startTime;
    TTF_Font* font;


    void processEvents();
    void update();
    void render();
    void loadNextLevel();
    void moveHole();
    void renderTime(SDL_Renderer* renderer);
};

#endif
