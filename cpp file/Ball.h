#ifndef BALL_H
#define BALL_H

#include <SDL.h>

class Ball {
public:
    Ball(int x, int y, int radius);
    void update();
    void render(SDL_Renderer* renderer);
private:
    int x, y, radius;
    int velocityX, velocityY;
};

#endif
