#include "Ball.h"

Ball::Ball(int x, int y, int radius) : x(x), y(y), radius(radius), velocityX(0), velocityY(0) {}

void Ball::update() {
    x += velocityX;
    y += velocityY;
}

void Ball::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect ballRect = { x - radius, y - radius, radius * 2, radius * 2 };
    SDL_RenderFillRect(renderer, &ballRect);
}

