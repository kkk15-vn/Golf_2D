#include "Menu.h"

Menu::Menu() {}

void Menu::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect menuRect = { 50, 50, 200, 100 };
    SDL_RenderFillRect(renderer, &menuRect);
}

