#include <iostream>

#include "GameEngine.hpp"

int main() {
    std::cout << "Hello, Lineman!" << std::endl;

    GameEngine game("config/assets.txt");
    game.run();

    return 0;
}
