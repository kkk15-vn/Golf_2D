#include "Game.h"
#include "Ball.h"
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
    Mix_FreeChunk(hitSound); // Giải phóng âm thanh
    Mix_CloseAudio(); // Đóng SDL_mixer
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

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tải âm thanh từ file
    hitSound = Mix_LoadWAV("Sounds/hit.wav");
    if (!hitSound) {
        std::cerr << "Failed to load hit sound: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tải hình nền menu
    SDL_Surface* menuSurface = IMG_Load("Images/background.png");
    if (!menuSurface) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
        return false;
    }
    menuBackground = SDL_CreateTextureFromSurface(renderer, menuSurface);
    SDL_FreeSurface(menuSurface); // Giải phóng surface sau khi tạo texture

    // Thiết lập vị trí các nút menu
    startButton = {300, 250, 200, 50};  // Vị trí & kích thước nút Start
    introButton = {300, 320, 200, 50};  // Nút Introduction
    exitButton  = {300, 390, 200, 50};   // Nút Exit

    return window && renderer;
}

void Game::run() {
    while (running) {
        processEvents();
        if (inMenu) {
            renderMenu(renderer); // Hiển thị menu nếu đang ở menu
        } else {
            update();
            render();
        }
        SDL_Delay(16);
    }
    std::cout << "Game Over! You ran out of time!" << std::endl;

}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        // Nếu trò chơi đã kết thúc, kiểm tra phím để chơi lại
        if (isGameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
            restartGame(); // Chơi lại khi nhấn "R"
        }
        if (inMenu && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mouseX = event.button.x;
            int mouseY = event.button.y;

            // Xử lý các nút trong menu
            SDL_Point mousePoint = {mouseX, mouseY};
            if (SDL_PointInRect(&mousePoint, &startButton)) {
                inMenu = false; // Bắt đầu game
            } else if (SDL_PointInRect(&mousePoint, &introButton)) {
                std::cout << "Introduction Screen (Chưa cài đặt)" << std::endl;
            } else if (SDL_PointInRect(&mousePoint, &exitButton)) {
                running = false; // Thoát game
            }
        }

        if (!inMenu) {
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouseClickCount++;
            }

            // Phát âm thanh khi chuột được nhả ra
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                if (hitSound) {
                    Mix_PlayChannel(-1, hitSound, 0); // Phát âm thanh đánh bóng
                }
                if (mouseClickCount >= 2) { // Sau 2 lần nhấp, di chuyển lỗ golf
                    moveHole();
                    mouseClickCount = 0;
                }
            }

            ball.handleEvent(event);
        }
    }
}

// Thiết lập màn mới
void Game::loadNextLevel() {
    // Đặt lại vị trí bóng và hố cho màn chơi mới
    ball.setPosition(400, 300);
    hole.setPosition(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT); // Hố ở vị trí ngẫu nhiên

    // Cập nhật thời gian màn chơi
    maxTime = std::max(10, maxTime - 5); // Giảm 2s mỗi màn, nhưng không dưới 10s
    startTime = SDL_GetTicks(); // Ghi lại thời điểm bắt đầu màn mới

    // Giữ nguyên vị trí chướng ngại vật, chỉ tăng tốc
    for (auto& obstacle : obstacles) {
    obstacle.setSpeedX(obstacle.getSpeedX() * 4); // Tăng tốc sau mỗi màn
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
    if (inMenu || isGameOver) return; // Không cập nhật khi đang ở menu hoặc game đã kết thúc

    ball.update(obstacles);
    forceLevel = ball.getForceLevel();

    for (auto& obstacle : obstacles) {
        obstacle.update();
    }

    Uint32 currentTime = SDL_GetTicks();
    remainingTime = maxTime - (currentTime - startTime) / 1000;

    // Nếu hết thời gian mà bóng chưa vào lỗ, kích hoạt màn hình "Game Over"
    if (remainingTime <= 0 && !ball.checkHoleCollision(hole)) {
        isGameOver = true; // Đánh dấu trạng thái kết thúc game
        std::cout << "Game Over! Time ran out!" << std::endl;
    }

    // Nếu bóng vào lỗ trước khi hết thời gian, tiếp tục chơi
    if (ball.checkHoleCollision(hole)) {
        currentLevel++;
        loadNextLevel();
    }
}
void Game::render() {
    // Vẽ nền xanh
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);

    // Kiểm tra xem game đã kết thúc chưa
    if (isGameOver) {
        renderGameOver(renderer); // Hiển thị màn hình kết thúc
        SDL_RenderPresent(renderer); // Cập nhật màn hình
        return; // Ngừng vẽ các thành phần khác
    }

    // Vẽ viền trắng
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 10; i++) {
        SDL_Rect border = {i, i, SCREEN_WIDTH - 2 * i, SCREEN_HEIGHT - 2 * i};
        SDL_RenderDrawRect(renderer, &border);
    }

    hole.render(renderer); // Vẽ hố golf
    ball.render(renderer); // Vẽ quả bóng
    ball.renderDragLine(renderer); // Vẽ tia kéo
    renderTime(renderer); // Hiển thị thời gian

    // Vẽ các chướng ngại vật
    for (const auto& obstacle : obstacles) {
        obstacle.render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::renderTime(SDL_Renderer* renderer) {
    if (remainingTime < 0) remainingTime = 0; // Tránh hiển thị giá trị âm

    SDL_Color textColor = {255, 255, 255, 255}; // Màu trắng

    // Hiển thị thời gian còn lại
    std::string timeText = "Time Left: " + std::to_string(remainingTime) + "s";
    SDL_Surface* surfaceTime = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
    SDL_Texture* textureTime = SDL_CreateTextureFromSurface(renderer, surfaceTime);
    SDL_Rect timeRect = {10, 10, surfaceTime->w, surfaceTime->h};
    SDL_RenderCopy(renderer, textureTime, nullptr, &timeRect);
    SDL_FreeSurface(surfaceTime);
    SDL_DestroyTexture(textureTime);

    // Hiển thị chữ "Power"
    std::string powerText = "Power";
    SDL_Surface* surfacePower = TTF_RenderText_Solid(font, powerText.c_str(), textColor);
    SDL_Texture* texturePower = SDL_CreateTextureFromSurface(renderer, surfacePower);
    SDL_Rect powerRect = {10, timeRect.y + timeRect.h + 5, surfacePower->w, surfacePower->h};
    SDL_RenderCopy(renderer, texturePower, nullptr, &powerRect);
    SDL_FreeSurface(surfacePower);
    SDL_DestroyTexture(texturePower);

    // Vẽ hình chữ nhật rỗng bên cạnh "Power"
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
    SDL_Rect powerBar = {powerRect.x + powerRect.w + 10, powerRect.y, 100, powerRect.h}; // Hình chữ nhật rỗng
    SDL_RenderDrawRect(renderer, &powerBar); // Vẽ viền hình chữ nhật

    // **Vẽ thanh đo lực bên trong hình chữ nhật rỗng**
    int forceWidth = (forceLevel * powerBar.w) / 100; // Đảm bảo chiều dài thanh lực đạt tối đa 100%
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
    SDL_Rect forceBar = {powerBar.x + 2, powerBar.y + 2, forceWidth - 4, powerBar.h - 4}; // Thanh đo lực
    SDL_RenderFillRect(renderer, &forceBar);
}
void Game::renderMenu(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, menuBackground, nullptr, nullptr); // Hiển thị nền

    // Hiển thị chữ trên các nút
    renderText("START", startButton.x + 50, startButton.y + 10);
    renderText("INTRODUCTION", introButton.x + 0, introButton.y + 10);
    renderText("EXIT", exitButton.x + 60, exitButton.y + 10);
    SDL_RenderPresent(renderer);
}

void Game::renderText(const std::string& text, int x, int y) {
    SDL_Color textColor = {255, 255, 255, 255}; // Màu trắng
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderGameOver(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nền màu đen
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 0, 0, 255}; // Màu đỏ
    renderText("GAME OVER", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 50);
    renderText("Press R to Restart", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 30);

    SDL_RenderPresent(renderer);
}

void Game::restartGame() {
    isGameOver = false;  // Đặt lại trạng thái game
    remainingTime = maxTime;  // Đặt lại thời gian
    ball.setPosition(startX, startY);  // Đặt lại vị trí bóng
    running = true;  // Tiếp tục vòng lặp game
    startTime = SDL_GetTicks();  // Đặt lại thời gian bắt đầu game
}
