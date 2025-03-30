#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

int ballX = 300;
int ballY = 300;
float ballVx = 0; // Vận tốc theo trục X
float ballVy = 0; // Vận tốc theo trục Y
float friction = 0.98f; // Ma sát để bóng dừng dần
int ballRadius = 10;
int endX = 0, endY = 0;
float shotPower = 0.0f; // Lực bắn bóng
int holeX = 500, holeY = 300, holeRadius = 12;// Tọa độ hố golf
bool dragging = false;
int startX, startY;
int currentMap = 0;
bool gameOver = false;

// Thời gian
Uint32 startTime = 0;
Uint32 levelTime = 0;
Uint32 totalTime = 0;


// Danh sách các map với tọa độ hố golf
std::vector<std::pair<int, int>> maps = {{600, 400}, {200, 500}, {700, 200}};

// Cấu trúc đại diện cho vật cản (hình chữ nhật)
struct Obstacle {
    int x, y, w, h;
};

// Danh sách các vật cản trong map
std::vector<Obstacle> obstacles = {
    {300, 250, 150, 30},  // Vật cản ngang
    {500, 350, 150, 30},  // Vật cản dọc
    {200, 400, 150, 30}   // Vật cản ngang
};

// Hàm load font
bool loadFont(const std::string& path, int size) {
    font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        std::cout << "Không thể tải font! Lỗi: " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}


// Hàm khởi tạo SDL
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Lỗi SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Golf 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Không thể tạo cửa sổ! Lỗi: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Không thể tạo renderer! Lỗi: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cout << "Lỗi SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!loadFont("c:/c++ codeblocks/golf_2d/bin/debug/arial.ttf", 24)) return false;

    startTime = SDL_GetTicks();  // Bắt đầu đếm thời gian từ màn đầu tiên
    return true;
}

// Hàm vẽ chữ
void renderText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}



// Hàm vẽ vật cản
void drawObstacles(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
    for (const auto& obs : obstacles) {
        SDL_Rect rect = {obs.x, obs.y, obs.w, obs.h};
        SDL_RenderFillRect(renderer, &rect);
    }
}


// Hàm vẽ hình tròn (quả bóng golf và hố golf)
void drawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}

// Kiểm tra xem bóng có vào hố không
bool checkWin() {
    int dx = ballX - holeX;
    int dy = ballY - holeY;
    return (dx * dx + dy * dy) <= (holeRadius * holeRadius);
}

// Hàm xử lý sự kiện chuột
void handleMouseEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        int dx = mouseX - ballX;
        int dy = mouseY - ballY;
        if (dx * dx + dy * dy <= ballRadius * ballRadius) {
            dragging = true;
            startX = mouseX;
            startY = mouseY;
        }
    }
    else if (e.type == SDL_MOUSEMOTION && dragging) {
        endX = e.motion.x;
        endY = e.motion.y;
        int dx = startX - endX;
        int dy = startY - endY;
        shotPower = sqrt(dx * dx + dy * dy) / 5.0f; // Tính lực (giới hạn khoảng 100)
        if (shotPower > 100) shotPower = 100;
    }
    else if (e.type == SDL_MOUSEBUTTONUP && dragging) {
        int endX = e.button.x;
        int endY = e.button.y;
        ballVx = (startX - endX) * 0.02f;  // Tính vận tốc theo lực kéo
        ballVy = (startY - endY) * 0.02f;
        dragging = false;
    }
}

// Kiểm tra va chạm bóng với vật cản
void checkCollision() {
    for (const auto& obs : obstacles) {
        if (ballX + ballRadius > obs.x && ballX - ballRadius < obs.x + obs.w &&
            ballY + ballRadius > obs.y && ballY - ballRadius < obs.y + obs.h) {

            float overlapX1 = (obs.x + obs.w) - (ballX - ballRadius);
            float overlapX2 = (ballX + ballRadius) - obs.x;
            float overlapY1 = (obs.y + obs.h) - (ballY - ballRadius);
            float overlapY2 = (ballY + ballRadius) - obs.y;

            float minOverlapX = std::min(overlapX1, overlapX2);
            float minOverlapY = std::min(overlapY1, overlapY2);

            if (minOverlapX < minOverlapY) {
                ballVx = -ballVx; // Đảo ngược vận tốc X
                ballX += (ballVx > 0) ? minOverlapX : -minOverlapX;
            } else {
                ballVy = -ballVy; // Đảo ngược vận tốc Y
                ballY += (ballVy > 0) ? minOverlapY : -minOverlapY;
            }
        }
    }
}


// Hàm cập nhật tốc độ bóng
void updateBall() {
    if (fabs(ballVx) > 0.1f || fabs(ballVy) > 0.1f) {
        ballX += ballVx;
        ballY += ballVy;
        ballVx *= friction;
        ballVy *= friction;
    } else {
        ballVx = 0;
        ballVy = 0;
    }

    // Reset bóng nếu ra khỏi màn hình
    if (ballX < 0 || ballX > SCREEN_WIDTH || ballY < 0 || ballY > SCREEN_HEIGHT) {
        ballX = 300;
        ballY = 300;
        ballVx = 0;
        ballVy = 0;
    }
}

// Hàm cập nhật vị trí vật cản
void MoveObstacles() {
    for (auto& obs : obstacles) {
        // Sinh vị trí mới gần hố golf (cách hố trong khoảng 50 - 150 pixels)
        int offsetX = (rand() % 100 + 50) * (rand() % 2 ? 1 : -1);

        obs.x = holeX + offsetX;

        // Đảm bảo không ra khỏi màn hình
        obs.x = std::max(0, std::min(obs.x, 600 - obs.w));
    }
}

// Hàm định hướng bóng
void drawDirectionLine(SDL_Renderer* renderer, int startX, int startY, int endX, int endY) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
    SDL_RenderDrawLine(renderer, startX, startY, endX, endY);
}

//Hàm vẽ thanh đo lực
void drawPowerBar(SDL_Renderer* renderer, int x, int y, int power) {
    int maxPower = 100;  // Giới hạn lực tối đa
    int barHeight = 100; // Chiều cao tối đa của thanh đo lực
    int filledHeight = (power * barHeight) / maxPower;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect barOutline = {x, y, 10, barHeight};  // Viền thanh đo
    SDL_RenderDrawRect(renderer, &barOutline);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect barFill = {x, y + (barHeight - filledHeight), 10, filledHeight};
    SDL_RenderFillRect(renderer, &barFill);
}

void nextMap() {
    levelTime = SDL_GetTicks() - startTime;  // Thời gian của màn hiện tại
    totalTime += levelTime;  // Cộng vào tổng thời gian
    currentMap++;

    if (currentMap >= maps.size()) {
        gameOver = true;  // Hoàn thành tất cả màn chơi
    } else {
        holeX = maps[currentMap].first;
        holeY = maps[currentMap].second;
        ballX = 300;
        ballY = 300;
        ballVx = 0;
        ballVy = 0;
        startTime = SDL_GetTicks();  // Bắt đầu tính thời gian cho màn mới
    }
}

void renderGame() {
    // Vẽ viền trắng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
        SDL_Rect borderRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &borderRect);

        // Vẽ nền
        SDL_SetRenderDrawColor(renderer, 0, 190, 0, 255);
        SDL_Rect fieldRect = {20, 20, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40};
        SDL_RenderFillRect(renderer, &fieldRect);

        // Vẽ hố golf
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        drawCircle(renderer, holeX, holeY, holeRadius);

        // Vẽ bóng golf
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCircle(renderer, ballX, ballY, ballRadius);

        for (const auto& obs : obstacles) {
            SDL_Rect rect = {obs.x, obs.y, obs.w, obs.h};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
            SDL_RenderFillRect(renderer, &rect);
        }


        // Vẽ tia hướng bóng
        if (dragging) {
        drawDirectionLine(renderer, ballX, ballY, endX, endY);
        drawPowerBar(renderer, 50, 50, shotPower);
        drawObstacles(renderer);
        }
    // Hiển thị thời gian
    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;  // Tính theo giây
    std::string timeText = "Time: " + std::to_string(elapsedTime) + "s";
    renderText(timeText.c_str(), 20, 20, {255, 255, 255});

    SDL_RenderPresent(renderer);
}

void renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};
    renderText("Game Over!", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, white);
    renderText("Press ESC to Exit", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 20, white);

    SDL_RenderPresent(renderer);
}


// Hàm giải phóng bộ nhớ
void close() {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) {
        std::cout << "Khởi tạo thất bại!" << std::endl;
        return -1;
    }

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_MOUSEBUTTONUP) MoveObstacles();  // Khi nhả chuột, chướng ngại vật sẽ di chuyển
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;

            if (!gameOver) handleMouseEvent(e);
        }

       if (!gameOver) {
            updateBall();
            checkCollision();
            if (checkWin()) nextMap();
            renderGame();
        } else {
            renderGameOver();
        }
    }

    close();
    return 0;
}
