#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Menu {
public:
    Menu();
    ~Menu();

    bool init(SDL_Renderer* renderer);
    void handleEvent(SDL_Event& e, bool& inGame, bool& running);
    void render(SDL_Renderer* renderer);

private:
    SDL_Texture* textTexture;
    SDL_Rect textRect;
};

#endif
