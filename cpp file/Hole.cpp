#include "Hole.h"

Hole::Hole(int x, int y, int radius) : x(x), y(y), radius(radius) {}

void Hole::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Hole::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}
