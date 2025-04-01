#include "Obstacle.h"
#include "Game.h"

Obstacle::Obstacle(int x, int y, int width, int height, int speedX)
    : x(x), y(y), width(width), height(height), speedX(speedX) {}

void Obstacle::render(SDL_Renderer* renderer) const{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(renderer, &rect);
}

void Obstacle::update() {
    x += speedX; // Di chuyển ngang

    // Nếu chạm biên, đảo chiều
    if (x <= 0 || x + width >= SCREEN_WIDTH) {
        speedX = -speedX;
    }
}

void Obstacle::setSpeedX(int newSpeedX) {
    speedX = newSpeedX;
}
