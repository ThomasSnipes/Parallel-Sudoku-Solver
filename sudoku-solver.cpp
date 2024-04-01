// Online C++ compiler to run C++ program online
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx; 

const int SIZE = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20; // Number of initially filled cells

// Function to check if a number is safe to place in the given position
bool isSafe(const vector<vector<int>>& board, int row, int col, int num) {
    // Check if the number is already present in the row or column
    for (int i = 0; i < SIZE; ++i) {
        if (board[row][i] == num || board[i][col] == num)
            return false;
    }

    // Check if the number is already present in the current 3x3 subgrid
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

// Function to print the Sudoku board
void printBoard(const vector<vector<int>>& board) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            cout << board[i][j] << " ";
            if ((j + 1) % SUBGRID_SIZE == 0 && j != SIZE - 1)
                cout << "| ";
        }
        cout << endl;
        if ((i + 1) % SUBGRID_SIZE == 0 && i != SIZE - 1) {
            for (int k = 0; k < SIZE + SUBGRID_SIZE - 1; ++k)
                cout << "--";
            cout << endl;
        }
    }
}

// Generate intial Sudoku board
void initializeBoard(vector<vector<int>>& board) {
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
// Recursive function to solve Sudoku using backtracking
bool solveSudoku(vector<vector<int>>& board, int row, int col) {
    if (row == SIZE - 1 && col == SIZE)
        return true;

    if (col == SIZE) {
        row++;
        col = 0;
    }

    if (board[row][col] != 0)
        return solveSudoku(board, row, col + 1);

    for (int num = 1; num <= SIZE; ++num) {
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;
            if (solveSudoku(board, row, col + 1))
                return true;
            board[row][col] = 0;
        }
    }

    return false;
}

// Function executed by each thread to solve Sudoku
void solveSudokuParallel(vector<vector<int>> board, int threadID) {
    if (solveSudoku(board, 0, threadID)) {
        mtx.lock();
        cout << "Thread " << threadID << " found a solution:" << endl;
        printBoard(board);
        mtx.unlock();
    } else {
        mtx.lock();
        cout << "Thread " << threadID << " did not find a solution." << endl;
        mtx.unlock();
    }
}

int main() {
    vector<vector<int>> board(SIZE, vector<int>(SIZE, 0));

    initializeBoard(board);

    cout << "Initial board:" << endl;
    printBoard(board);
    
    // Create threads to solve Sudoku in parallel
    vector<thread> threads;
    for (int i = 0; i < SIZE; ++i) {
        threads.emplace_back(solveSudokuParallel, board, i);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
