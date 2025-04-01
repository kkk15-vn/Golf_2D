#ifndef BALL_H
#define BALL_H

#include <SDL.h>
#include <vector>
#include "Obstacle.h"
#include "Hole.h"

class Ball {
public:
    Ball(int x, int y, int radius);
    void handleEvent(SDL_Event& event); // Xử lý chuột
    void update(const std::vector<Obstacle>& obstacles);
    void render(SDL_Renderer* renderer);
    void renderDragLine(SDL_Renderer* renderer); // Vẽ tia kéo
    void renderForceBar(SDL_Renderer* renderer);
    bool checkCollision(const Obstacle& obstacle);
    bool checkHoleCollision(const Hole& hole);
    void setPosition(int newX, int newY);
private:
    int x, y, radius;
    int velocityX, velocityY;
    int startX, startY; // Vị trí chuột bắt đầu kéo
    int endX, endY; // Vị trí chuột khi thả
    int forceLevel; // Lưu giá trị lực kéo
    bool isDragging;

};

#endif
