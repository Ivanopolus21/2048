//
// Created by yeremiva on 10.12.2023.
//
#include <iostream>
#include <cstdlib>
#include <thread>

#include "game.hpp"

using namespace std;

const int MAX = 2048;
int board [4][4]={0};
int step; /**< Tracks the number of steps taken in the game. */
int score; /**< Tracks the player's score. */
bool isContinue; /**< Indicates whether the player chose to continue after winning. */

void Game::startTheGame() {

    thread input(&Game::inputThread, this);
    thread output(&Game::outputThread, this);
    thread compute(&Game::computeThread, this);

    input.join();
    output.join();
    compute.join();
}

void Game::inputThread() {
    while (true) {
        if (!isWin() || isContinue) {
            handleUserInput();
        }

        unique_lock<mutex> lock(mtx);

        if (quitRequested) {
            lock.unlock();
            break;
        }

        inputCvar.notify_one();
        lock.unlock();
    }
}

void Game::outputThread() {
    unique_lock<mutex> lock(mtx);
    while (!quitRequested) {
        outputCvar.wait(lock);
        display();
    }
}

void Game::computeThread() {
    unique_lock<mutex> lock(mtx);
    outputCvar.notify_one();
    lock.unlock();

    while (!quitRequested) {
        unique_lock<mutex> lock(mtx);
        inputCvar.wait(lock);
        if (isWin() && !isContinue) {
            handleWin();

        } else if (gameOver()) {
            handleGameOver();
        } else {
            outputCvar.notify_one();
        }
        lock.unlock();
    }
    outputCvar.notify_all();
    inputCvar.notify_all();
}

void Game::handleWin() {
    display();
    cout<<step<<" steps"<<endl;
    cout<<"!!!YOU WIN!!!"<<endl;
    cout<<"Continue? (y/n)"<<endl;

    showChoiceAfterTheEnd();
}

void Game::handleGameOver() {
    display();
    cout<<"~~~GAME OVER~~~"<<endl;
    exit(2);
}

bool Game::isWin() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == MAX) {
                return true;
            }
        }
    }
    return false;
}


bool Game::gameOver(){
    bool isGameOver = true;
    for(auto & cell : board) {
        for(int j = 0; j < 3; j++) {
            if(cell[j] == 0 || cell[j+1] == 0 || cell[j] == cell[j+1]) {
                isGameOver = false;
                break;
            }
        }
    }
    for(int j = 0; j < 4; j++) {
        for(int i = 0; i < 3; i++) {
            if(board[i][j] == 0 || board[i+1][j] == 0 || board[i][j] == board[i+1][j]) {
                isGameOver = false;
                break;
            }
        }
    }
    return isGameOver;
}


void Game::display(){
    int index_i1, index_j1, index_i2, index_j2;

    while(true) {
        index_i1 = randomIndexGenerate();
        index_j1 = randomIndexGenerate();
        index_i2 = randomIndexGenerate();
        index_j2 = randomIndexGenerate();

        if(index_i1 == index_i2 && index_j1 == index_j2) {
            continue;
        } else {
            break;
        }
    }

    //initialize
    if(step == 0) {
        for(int i = 0; i < 4; i++) {
            cout<<"|-----------------------|"<<endl;

            for(int j = 0; j < 4; j++) {
                cout<<"|";
                if(i == index_i1 && j == index_j1) {
                    board[i][j] = 2;
                    cout<<"  "<<2<<"  ";
                } else if(i == index_i2 && j == index_j2) {
                    int temp = newRandomElement();
                    board[i][j] = temp;
                    cout<<"  "<<temp<<"  ";
                } else {
                    cout<<"     ";
                }
            }
            cout<<"|"<<endl;
        }
        cout<<"|-----------------------|"<<endl;
    } else {     //redisplay
        cout<<endl<<endl;
        for(auto & cell : board) {
            cout<<"|-----------------------|"<<endl;
            for(int j : cell) {
                cout<<"|";
                if(j != 0) {
                    if(j == 1024 || j == 2048) {
                        cout<<" "<<j;
                    }
                    if(j == 128 || j == 256 || j == 512) {
                        cout<<" "<<j<<" ";
                    }
                    if(j == 16 || j == 32 || j == 64) {
                        cout<<"  "<<j<<" ";
                    }
                    if(j == 2 || j == 4 || j == 8) {
                        cout<<"  "<<j<<"  ";
                    }
                } else {
                    cout<<"     ";
                }
            }
            cout<<"|"<<endl;
        }
        cout<<"|-----------------------|"<<endl;
    }
    cout<<"score: "<<score<<endl<<endl;
}

void Game::addElement() {
    int index_i3, index_j3;
    bool flag = false;
    while(true) {
        if(flag) {
            break;
        }
        index_i3 = randomIndexGenerate();
        index_j3 = randomIndexGenerate();
        if(board[index_i3][index_j3] == 0){
            board[index_i3][index_j3] = newRandomElement();
            flag = true;
        }
    }
}

void Game::mergeTiles(char state, int numFlag) {
    for(int i = 0; i < 4; i++) {
        int n = 0;
        int prev = 0;
        switch (state) {
            case 'u':
                for (int j = 0; j < 4; j++) {
                    if (n == board[j][i] && n != 0){
                        board[prev][i] = 2 * n;
                        board[j][i] = 0;
                        score += 2 * n;
                        n = 0;
                        numFlag++;
                        continue;
                    }
                    if (board[j][i] != 0){
                        n = board[j][i];
                        prev = j;
                    }
                }
                break;
            case 'd':
                for (int j = 3; j >= 0; j--)
                {
                    if (n == board[j][i] && n != 0){
                        board[prev][i] = 2 * n;
                        board[j][i] = 0;
                        score += 2 * n;
                        n = 0;
                        numFlag++;
                        continue;
                    }
                    if (board[j][i] != 0){
                        n = board[j][i];
                        prev = j;
                    }
                }
                break;
            case 'l':
                for (int j = 0; j < 4; j++) {
                    if (n == board[i][j] && n != 0){
                        board[i][prev] = 2 * n;
                        board[i][j] = 0;
                        score += 2 * n;
                        n = 0;
                        numFlag++;
                        continue;
                    }
                    if (board[i][j] != 0){
                        n = board[i][j];
                        prev = j;
                    }
                }
                break;
            case 'r':
                for (int j = 3; j >= 0; j--)
                {
                    if (n == board[i][j] && n != 0){
                        board[i][prev] = 2 * n;
                        board[i][j] = 0;
                        score += 2 * n;
                        n = 0;
                        numFlag++;
                        continue;
                    }
                    if (board[i][j] != 0){
                        n = board[i][j];
                        prev = j;
                    }
                }
                break;
        }
    }
}

void Game::moveInDirection(char state) {
    int numFlag = 0;

    mergeTiles(state, numFlag);

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            switch(state) {
                case 'u':
                    for(int k = 0; k < 3; k++){
                        if(board[k][i] == 0 && board[k+1][i] != 0){
                            board[k][i] = board[k][i]^board[k+1][i];
                            board[k+1][i] = board[k][i]^board[k+1][i];
                            board[k][i] = board[k][i]^board[k+1][i];

                            numFlag++;
                        }
                    }
                    break;
                case 'd':
                    for(int k = 3; k > 0; k--){
                        if(board[k][i] == 0 && board[k-1][i] != 0){
                            board[k][i] = board[k][i]^board[k-1][i];
                            board[k-1][i] = board[k][i]^board[k-1][i];
                            board[k][i] = board[k][i]^board[k-1][i];

                            numFlag++;
                        }
                    }
                    break;
                case 'l':
                    for(int k = 0; k < 3; k++) {
                        if(board[i][k] == 0 && board[i][k+1] != 0) {
                            board[i][k] = board[i][k]^board[i][k+1];
                            board[i][k+1] = board[i][k]^board[i][k+1];
                            board[i][k] = board[i][k]^board[i][k+1];

                            numFlag++;
                        }
                    }
                    break;
                case 'r':
                    for(int k = 3; k > 0; k--){
                        if(board[i][k] == 0 && board[i][k-1] != 0){
                            board[i][k] = board[i][k]^board[i][k-1];
                            board[i][k-1] = board[i][k]^board[i][k-1];
                            board[i][k] = board[i][k]^board[i][k-1];

                            numFlag++;
                        }
                    }
                    break;
            }
        }
    }

    if(numFlag != 0) {
        addElement();
        step++;
    }

}

void Game::showChoiceAfterTheEnd() {
    char c;
    do {
        cin>>c;
        if (c == 'n' || c == 'y') {
            break;
        }
        cout<<"Invalid input."<<endl;
    } while(true);

    if(c == 'n') {
        exit(1);
    } else if (c == 'y') {
        isContinue = true;
    }
}

void Game::handleUserInput() {
    char ch;
    cin>>ch;
    switch (ch) {
        case 'w':
        case 'W':
            moveInDirection('u');
            break;
        case 's':
        case 'S':
            moveInDirection('d');
            break;
        case 'a':
        case 'A':
            moveInDirection('l');
            break;
        case 'd':
        case 'D':
            moveInDirection('r');
            break;
        case 'q':
        case 'Q':
            exit(1);
        case '!':
            handleWin();  ///only for debugging! Instant win. Check a documentation for more
            break;
        case '?':
            handleGameOver(); ///only for debugging! Instant lose. Check a documentation for more
            break;
        default:
            cout<<"Invalid input."<<endl;
            break;
    }
}

int Game::randomIndexGenerate() {
    int randomIndex = rand() % 4;
    return randomIndex;
}

int Game::newRandomElement() {
    int randomElement = rand() % 10;
    randomElement = (randomElement == 0) ? 4 : 2;
    return randomElement;
}

void Game::printHelp() {
    cout<<"------2048------"<<endl;
    cout<<"Rules:"<<endl;
    cout<<"Use w(up), s(down), a(left), d(right) keys to move the tiles in certain direction"<<endl;
    cout<<"Double up and merge titles with the same number touch"<<endl;
    cout<<"Type q to quit the program"<<endl<<endl;
}
