//
// Created by yeremiva on 10.12.2023.
//

#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

/**
 * @class Game
 * @brief Represents the 2048 game.
 */
class Game {
public:

    /**
     * @brief Starts the game by launching input, output, and compute threads.
     */
    void startTheGame();

    /**
     * @brief Prints help and rules for playing the game.
     */
    void printHelp();
private:
    void inputThread(); /**< Thread for handling user input. */
    void outputThread(); /**< Thread for displaying the game output. */
    void computeThread(); /**< Thread for computing game logic. */

    mutex mtx; /**< Mutex for thread synchronization. */

    condition_variable inputCvar; /**< Condition variable for input thread synchronization. */
    condition_variable outputCvar; /**< Condition variable for output thread synchronization. */

    bool quitRequested = false; /**< Indicates whether the game should quit. */

    /**
     * @brief Handles user input during the game.
     */
    void handleUserInput();

    /**
     * @brief Displays the game board and score.
     */
    void display();

    /**
     * @brief Checks if the player has won.
     * @return True if the player has won, false otherwise.
     */
    bool isWin();

    /**
     * @brief Checks if the game is over.
     * @return True if the game is over, false otherwise.
     */
    bool gameOver();

    /**
     * @brief Handles the win scenario.
     */
    void handleWin();

    /**
     * @brief Handles the game over scenario.
     */
    void handleGameOver();

    /**
     * @brief Displays a choice prompt after the game ends.
     */
    void showChoiceAfterTheEnd();

    /**
     * @brief Inserts a random element into the game board.
     */
    void addElement();

    /**
     * @brief Merges tiles in a specified direction.
     * @param state The direction ('u', 'd', 'l', 'r') in which tiles should be merged.
     * @param numFlag The number of merged tiles.
     */
    void mergeTiles(char state, int numFlag);

    /**
     * @brief Moves tiles in a specified direction.
     * @param state The direction ('u', 'd', 'l', 'r') in which tiles should be moved.
     */
    void moveInDirection(char state);

    /**
     * @brief Generates a random index for the game board.
     * @return Random index (0-3).
     */
    int randomIndexGenerate();

    /**
     * @brief Generates a new random element (2 or 4) for the game board.
     * @return Random element (2 or 4).
     */
    int newRandomElement();
};

#endif //GAME_HPP
