#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), running(true),
               currentLevel(1),
               ball(400, 300, 10),
               hole(700, 500, 12),
               maxTime(30),
               startTime(SDL_GetTicks()) {
               // Thêm chướng ngại vật vào danh sách
    obstacles.push_back(Obstacle(300, 250, 150, 30, 1));
    obstacles.push_back(Obstacle(500, 350, 150, 30, -1));
    obstacles.push_back(Obstacle(200, 400, 150, 30, 1));
}
Game::~Game() {
    TTF_CloseFont(font); // Giải phóng font
    TTF_Quit(); // Đóng SDL_ttf
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }
    if (TTF_Init() == -1) { // Khởi tạo SDL_ttf
        return false;
    }

    window = SDL_CreateWindow("Golf 2D",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Khởi tạo font chữ
    font = TTF_OpenFont("Fonts/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }


    return window && renderer;
}

void Game::run() {
    while (running) {
        processEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) { // Nếu chuột được nhấp
            mouseClickCount++;

            if (mouseClickCount >= 2) { // Sau 2 lần nhấp, di chuyển lỗ golf
                moveHole();
                mouseClickCount = 0; // Đặt lại bộ đếm
            }
        }

        ball.handleEvent(event);
    }
}

// Thiết lập màn mới
void Game::loadNextLevel() {
    // Đặt lại vị trí bóng và hố cho màn chơi mới
    ball.setPosition(400, 300);
    hole.setPosition(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT); // Hố ở vị trí ngẫu nhiên

    // Cập nhật thời gian màn chơi
    maxTime = std::max(10, maxTime - 2); // Giảm 2s mỗi màn, nhưng không dưới 10s
    startTime = SDL_GetTicks(); // Ghi lại thời điểm bắt đầu màn mới

    // Giữ nguyên vị trí chướng ngại vật, chỉ tăng tốc
    for (auto& obstacle : obstacles) {
    obstacle.setSpeedX(obstacle.getSpeedX() * 1.05); // Tăng tốc 20% sau mỗi màn
    }

    // Đảm bảo hố không xuất hiện quá gần biên
    int safeMargin = 50;
    int holeX = safeMargin + rand() % (SCREEN_WIDTH - 2 * safeMargin);
    int holeY = safeMargin + rand() % (SCREEN_HEIGHT - 2 * safeMargin);
    hole.setPosition(holeX, holeY);

}

void Game::moveHole() {
    int safeMargin = 50;
    int holeX = safeMargin + rand() % (SCREEN_WIDTH - 2 * safeMargin);
    int holeY = safeMargin + rand() % (SCREEN_HEIGHT - 2 * safeMargin);

    hole.setPosition(holeX, holeY); // Cập nhật vị trí mới
}

// Cập nhật trạng thái game
void Game::update() {
    ball.update(obstacles);
    for (auto& obstacle : obstacles) {
        obstacle.update(); // Cập nhật vị trí của vật thể
    }
    Uint32 currentTime = SDL_GetTicks();
    remainingTime = maxTime - (currentTime - startTime) / 1000; // Tính thời gian còn lại

    if (remainingTime <= 0) { // Nếu hết thời gian
        loadNextLevel();
    }

   // Kiểm tra xem bóng đã vào lỗ chưa
    if (ball.checkHoleCollision(hole)) {
        currentLevel++;  // Tăng màn chơi khi bóng vào lỗ
        loadNextLevel(); // Gọi hàm tải màn mới
    }
}

void Game::render() {
    // Vẽ nền xanh
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ viền trắng
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 10; i++) {
        SDL_Rect border = {i, i, SCREEN_WIDTH - 2 * i, SCREEN_HEIGHT - 2 * i};
        SDL_RenderDrawRect(renderer, &border);
    }
    for (const auto& obstacle : obstacles) {
        obstacle.render(renderer);
    }

    hole.render(renderer); // Vẽ hố golf
    ball.render(renderer); // Vẽ quả bóng
    ball.renderDragLine(renderer); // Vẽ tia kéo
    ball.renderForceBar(renderer); // Vẽ thanh đo lực
    renderTime(renderer); // Gọi hàm hiển thị thời gian

    // Vẽ các chướng ngại vật

    SDL_RenderPresent(renderer);
}

void Game::renderTime(SDL_Renderer* renderer) {
    if (remainingTime < 0) remainingTime = 0; // Tránh hiển thị giá trị âm

    SDL_Color textColor = {255, 255, 255, 255}; // Màu trắng
    std::string timeText = "Time Left: " + std::to_string(remainingTime) + "s";

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect = {10, 10, surfaceMessage->w, surfaceMessage->h};
    SDL_RenderCopy(renderer, message, nullptr, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}
