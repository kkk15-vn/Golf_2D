#include "Obstacle.h"

Obstacle::Obstacle(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}

void Obstacle::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = { x, y, width, height };
    SDL_RenderFillRect(renderer, &rect);
}
