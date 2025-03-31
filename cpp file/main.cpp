#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Golf 2D", 800, 600)) {
        return -1;
    }
    game.run();
    game.cleanup();
    return 0;
}
