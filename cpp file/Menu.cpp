#include "Menu.h"
#include <iostream>

Menu::Menu() : textTexture(nullptr) {}

Menu::~Menu() {
    SDL_DestroyTexture(textTexture);
}

bool Menu::init(SDL_Renderer* renderer) {
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("Fonts/arial.ttf", 24);
    if (!font) {
        std::cout << "Không thể tải font! Lỗi: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Nhấn ENTER để chơi!", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {250, 250, surface->w, surface->h};
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);

    return true;
}

void Menu::handleEvent(SDL_Event& e, bool& inGame, bool& running) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
        inGame = true;
    } else if (e.type == SDL_QUIT) {
        running = false;
    }
}

void Menu::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
}
