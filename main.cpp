#include <iostream>
#include <string>
#include "C:\\Users\\User\\Desktop\\chess-library-master\\include\\chess.hpp"
#include "Search.h"

using namespace chess;
using namespace std;

std::vector<Move> MovesPlayer;

// Function to get user input and print it
string getUserInput() {
    string userInput;

    cout << "Enter your move: ";
    getline(cin, userInput);

    cout << "You entered: " << userInput << endl;
    return userInput;
}

int main() {
    // Initialize the board
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    cout << "Started Game";


    for (int i = 0; i < 60; i++) {
        // Make your bot's move
        pair<int, Move> result = SearchStart(board);
        Move botMove2 = result.second;
        board.makeMove(botMove2);
        MovesPlayer.push_back(botMove2);

        cout << "Dimibot: " << botMove2 << '\n';
        cout << board << "Updated Board" << '\n';

        if (board.isGameOver().first == GameResultReason::CHECKMATE || board.isGameOver().first == GameResultReason::THREEFOLD_REPETITION) {
            cout << "Checkmate! Game Over!" << endl;
            break;
        }




        Move userInput;
        userInput = uci::parseSan(board,getUserInput());
        board.makeMove(userInput);

        MovesPlayer.push_back(userInput);
        cout << "Updated Board after user's move: " << '\n';
        cout << board << '\n';

        if (board.isGameOver().first == GameResultReason::CHECKMATE || board.isGameOver().first == GameResultReason::THREEFOLD_REPETITION) {
            cout << "Checkmate! Game Over!" << endl;
            break;
        }
    }

    cout << "Moves played so far: ";
    for (const Move &move : MovesPlayer) {
        cout << move << " ";
    }
    cout << endl;

    return 0;
}