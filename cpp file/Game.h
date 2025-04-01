#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "Ball.h"
#include "Hole.h"
#include "Obstacle.h"
#include <vector>
#include <string>

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

    SDL_Texture* menuBackground; // Hình nền menu
    SDL_Rect startButton;       // Nút Start
    SDL_Rect introButton;       // Nút Introduction
    SDL_Rect exitButton;        // Nút Exit
    bool inMenu = true;         // Trạng thái menu

    bool running;
    Ball ball;
    Hole hole;
    std::vector<Obstacle> obstacles;// Mảng 3 chướng ngại vật
    int currentLevel;
    int mouseClickCount = 0;
    int forceLevel = 0; // Lưu giá trị lực kéo
    int remainingTime;
    int maxTime;
    Uint32 startTime;
    TTF_Font* font;
    Mix_Chunk* hitSound;

    void processEvents();
    void update();
    void render();
    void loadNextLevel();
    void moveHole();
    void renderTime(SDL_Renderer* renderer);
    void renderMenu(SDL_Renderer* renderer);
    void renderText(const std::string& text, int x, int y);
};

#endif
