#ifndef HOLE_H
#define HOLE_H

#include <SDL.h>

class Hole {
public:
    Hole(int x, int y, int radius);
    void setPosition(int newX, int newY);
    int getX() const { return x; }
    int getY() const { return y; }
    int getRadius() const { return radius; }
    void render(SDL_Renderer* renderer);
private:
    int x, y, radius;
};

#endif
