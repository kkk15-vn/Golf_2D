#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL.h>

class Obstacle {
public:
    Obstacle(int x, int y, int width, int height, int speedX);
    void render(SDL_Renderer* renderer) const;
    void update();
    void setSpeedX(int newSpeedX);
    int getSpeedX() const { return speedX; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
private:
    int x, y, width, height;
    int speedX;
};

#endif
