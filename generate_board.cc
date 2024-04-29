#include <iostream>
#include <vector>
#include <random>

const int SIZE = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 0;


using namespace std;


// Check if a number is safe to place in the given position
bool isSafe(const vector<vector<int> >& board, int row, int col, int num) {

    // Check if the number is already in the row or column
    for (int i = 0; i < SIZE; ++i) {
        if (board[row][i] == num || board[i][col] == num)
            return false;
    }

    // Check if the number is already in the current subgrid
    int startRow = row - row % SUBGRID_SIZE;
    int startCol = col - col % SUBGRID_SIZE;

    for (int i = 0; i < SUBGRID_SIZE; ++i) {
        for (int j = 0; j < SUBGRID_SIZE; ++j) {
            if (board[i + startRow][j + startCol] == num)
                return false;
        }
    }
    return true;
}

void initializeBoard(vector<vector<int> >& board) {
    srand(time(0));
    int count = NUM_STARTING_CELLS;

    while (count > 0) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        int num = rand() % SIZE + 1;

        if (board[row][col] == 0 && isSafe(board, row, col, num)) {
            board[row][col] = num;
            count--;
        }
    }
}

void printBoard(const vector<vector<int> >& board) {
    cout << "{";
    for (int i = 0; i < SIZE; ++i) {
        cout << "{";
        for (int j = 0; j < SIZE; ++j) {
            if (j == SIZE - 1)
                cout << board[i][j] << "}," << endl;
            else
                cout << board[i][j] << ", ";
        }
        // if ((i + 1) % SUBGRID_SIZE == 0 && i != SIZE - 1) {
        //     for (int k = 0; k < SIZE + SUBGRID_SIZE - 1; ++k)
        //         cout << "--";
        //     cout << endl;
        // }
    }
    cout << "}" << endl;
}

int main() {
    vector<vector<int> > board(SIZE, vector<int>(SIZE, 0));

    initializeBoard(board); printBoard(board);
    return 0;
}


