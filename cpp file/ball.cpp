#include "Ball.h"
#include "Game.h"
#include <vector>
#include <cmath>


Ball::Ball(int x, int y, int radius)
    : x(x), y(y), radius(radius), velocityX(0), velocityY(0), isDragging(false) {}

void Ball::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int dx = event.button.x - x;
        int dy = event.button.y - y;

        // Kiểm tra nếu nhấn vào bóng (tính theo tâm bóng)
        if ((dx * dx + dy * dy) <= (radius * radius)) {
            isDragging = true;
            startX = event.button.x;
            startY = event.button.y;
            endX = startX;
            endY = startY;  // Ban đầu, endX = startX
        }
    }
    else if (event.type == SDL_MOUSEMOTION && isDragging) {
        // Cập nhật vị trí chuột để hiển thị đường kéo
        endX = event.motion.x;
        endY = event.motion.y;
        // Tính toán lực kéo dựa trên khoảng cách giữa điểm bắt đầu và hiện tại
        forceLevel = std::min(100, static_cast<int>(sqrt(pow(startX - endX, 2) + pow(startY - endY, 2))));
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        if (isDragging) {
            isDragging = false;

            // Tính vận tốc dựa vào khoảng cách kéo chuột
            velocityX = (startX - endX) / 12.0;
            velocityY = (startY - endY) / 12.0;
        }
    }
}

void Ball::update(const std::vector<Obstacle>& obstacles) {
    if (!isDragging) { // Chỉ cập nhật nếu không kéo chuột
        x += velocityX;
        y += velocityY;

        // Giảm tốc độ theo thời gian (mô phỏng ma sát)
        velocityX *= 0.98;
        velocityY *= 0.98;

        // Kiểm tra va chạm với từng vật cản
        for (const auto& obstacle : obstacles) {
            if (checkCollision(obstacle)) {
                velocityX += obstacle.getSpeedX();
            }
        }

         // Kiểm tra va chạm với viền màn hình
        if (x - radius < 0) { // Chạm viền trái
            x = radius;
            velocityX = -velocityX * 0.8; // Bật ngược lại, giảm lực
        }
        if (x + radius > SCREEN_WIDTH) { // Chạm viền phải
            x = SCREEN_WIDTH - radius;
            velocityX = -velocityX * 0.8;
        }
        if (y - radius < 0) { // Chạm viền trên
            y = radius;
            velocityY = -velocityY * 0.8;
        }
        if (y + radius > SCREEN_HEIGHT) { // Chạm viền dưới
            y = SCREEN_HEIGHT - radius;
            velocityY = -velocityY * 0.8;
        }

        // Ngăn bóng trôi mãi mãi
        if (fabs(velocityX) < 0.1) velocityX = 0;
        if (fabs(velocityY) < 0.1) velocityY = 0;
    }
}

bool Ball::checkCollision(const Obstacle& obstacle) {
    int obstacleX = obstacle.getX();
    int obstacleY = obstacle.getY();
    int obstacleWidth = obstacle.getWidth();
    int obstacleHeight = obstacle.getHeight();

    // Kiểm tra va chạm dựa trên vị trí
    if (x + radius > obstacleX && x - radius < obstacleX + obstacleWidth &&
        y + radius > obstacleY && y - radius < obstacleY + obstacleHeight) {

        // Tạo phản lực dựa trên tốc độ vật thể
        int pushX = (obstacle.getSpeedX() > 0) ? 1 : -1;
        velocityX = pushX * std::abs(obstacle.getSpeedX()) * 1.5; // Tăng lực phản xạ

        return true; // Va chạm xảy ra
    }
    return false;
}

// Hàm  kiểm tra bóng vào lỗ
bool Ball::checkHoleCollision(const Hole& hole) {
    int dx = x - hole.getX();
    int dy = y - hole.getY();
    int distanceSquared = dx * dx + dy * dy;

    return distanceSquared <= (hole.getRadius() * hole.getRadius());
}
void Ball::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}


void Ball::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng

    for (int w = -radius; w < radius; w++) {
        for (int h = -radius; h < radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}

void Ball::renderDragLine(SDL_Renderer* renderer) {
    if (isDragging) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
        SDL_RenderDrawLine(renderer, x, y, endX, endY); // Tia từ bóng đến vị trí chuột
    }
}
