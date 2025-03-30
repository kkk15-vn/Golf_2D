while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            handleMouseEvent(e);
            if (e.type == SDL_MOUSEBUTTONUP) {
                MoveObstacles(); // Khi nhả chuột, chướng ngại vật sẽ di chuyển
            }
        }

        updateBall();
        checkCollision();

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
        // Kiểm tra nếu bóng vào hố
        if (checkWin()) {
            nextMap();
        }

        SDL_RenderPresent(renderer);
    }



 void checkCollision() {
    for (const auto& obs : obstacles) {
        // Kiểm tra nếu bóng nằm trong vùng vật cản
        if (ballX + ballRadius > obs.x && ballX - ballRadius < obs.x + obs.w &&
            ballY + ballRadius > obs.y && ballY - ballRadius < obs.y + obs.h) {

            // Nếu va chạm từ bên trái hoặc bên phải, đảo ngược vận tốc X
            if (ballX < obs.x || ballX > obs.x + obs.w) {
                ballVx = -ballVx;
            }

            // Nếu va chạm từ trên hoặc dưới, đảo ngược vận tốc Y
            if (ballY < obs.y || ballY > obs.y + obs.h) {
                ballVy = -ballVy;
            }
        }
    }
}



void updateBall() {
    if (fabs(ballVx) > 0.1f || fabs(ballVy) > 0.1f) { // Nếu bóng vẫn còn di chuyển
        ballX += ballVx;
        ballY += ballVy;
        // Giảm tốc do ma sát
        ballVx *= friction;
        ballVy *= friction;
    } else {
        ballVx = 0;
        ballVy = 0;
    }
}

Uint32 levelTime = 0;
Uint32 totalTime = 0;
