#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;

const int N = 9;

bool isValid(vector<vector<int>>& board, int row, int col, int num) {
    //Check row and col
    for (int i = 0; i < N; ++i) {
        if (board[row][i] == num || board[i][col] == num)
            return false;
    }
    
    // Check if the number is already present in the 3x3 box
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    //loop over local grid
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i + startRow][j + startCol] == num)
                return false;
        }
    }
    return true;
}

bool solveSudokuUtil(vector<vector<int>>& board, int row, int col) {
    //At end of grid
    if (row == N - 1 && col == N)
        return true;

    // Move to the next row if the column index has reached the end
    if (col == N) {
        row++;
        col = 0;
    }

    //move to next cell if current is occupied
    if (board[row][col] != 0)
        return solveSudokuUtil(board, row, col + 1);

    //range of possible numbers is 1-9
    for (int num = 1; num <= 9; ++num) {
        // Check if it's safe to place 'num' in the current cell
        if (isValid(board, row, col, num)) {
            // Place the number and recursively solve for the next cell
            board[row][col] = num;

            // Parallelize this part
            #pragma omp parallel sections
            {
                #pragma omp section
                if (solveSudokuUtil(board, row, col + 1))
                    return true;
            }

            //begin backtrack
            board[row][col] = 0;
        }
    }
    return false; // No solution found
}

bool solveSudoku(vector<vector<int>>& board) {
    //measure perf
    auto start_time = std::chrono::high_resolution_clock::now();
    bool res = solveSudokuUtil(board, 0, 0);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "runtime duration:: " << duration << std::endl;
    return res;
}

void printBoard(const vector<vector<int>>& board) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    vector<vector<int>> board = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    if (solveSudoku(board)) {
        cout << "Sudoku solved successfully:\n";
        printBoard(board);
    } else {
        cout << "No solution exists for the given Sudoku.\n";
    }
    return 0;
}