#include "Game.h"

int main(int argc, char* argv[]) {
    Game game; // Tạo đối tượng Game

    if (!game.init()) { // Khởi tạo game
        return -1;
    }

    game.run(); // Bắt đầu vòng lặp game

    return 0;
}
