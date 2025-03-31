#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL.h>

class Obstacle {
public:
    Obstacle(int x, int y, int w, int h);
    void render(SDL_Renderer* renderer);
private:
    int x, y, width, height;
};

#endif
