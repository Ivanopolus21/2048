#include <iostream>
#include <cstdlib>
#include "src/game.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    Game game;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        // Display help information
        if (argc == 2 && string(argv[1]) == "--help") {
            game.printHelp();
            return EXIT_SUCCESS;
        }
    }

    game.startTheGame();
}
