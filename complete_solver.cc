#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int num_threads = 5;
const int NUM_STARTING_CELLS = 20;
const int MAX = 10000;
int iter = 0;

vector<vector<int> > board(N, vector<int>(N, 0));
vector<vector<vector<int>>> solutions;

// Function to print the Sudoku board
void printBoard(vector<vector<int>> board) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << board[i][j] << " ";
            if ((j + 1) % SUBGRID_SIZE == 0 && j != N - 1)
                cout << "| ";
        }
        cout << endl;
        if ((i + 1) % SUBGRID_SIZE == 0 && i != N - 1) {
            for (int k = 0; k < N + SUBGRID_SIZE - 1; ++k)
                cout << "--";
            cout << endl;
        }
    }
}

bool isValid(int row, int col, int num) {
    //Check row and col
    for (int i = 0; i < N; ++i) {
        if (board[row][i] == num || board[i][col] == num)
            return false;
    }
    
    // Check if the number is already present in the 3x3 box
    int startRow = row - row % 3;
    int startCol = col - col % 3;

    // Loop over local grid
    for (int i = 0; i < SUBGRID_SIZE; ++i) {
        for (int j = 0; j < SUBGRID_SIZE; ++j) {
            if (board[i + startRow][j + startCol] == num)
                return false;
        }
    }
    return true;
}



//find the first empty location in the grid
bool findEmptyPlace(int &row, int &col){
   for (row = 0; row < N; row++)
      for (col = 0; col < N; col++)
         if (board[row][col] == 0) //marked with 0 is empty
            return true;
   return false;
}

bool boardsEqual(const vector<vector<int>>& board1, const vector<vector<int>>& board2) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (board1[i][j] != board2[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool solveSudoku() {
    int row;
    int col;
    iter += 1;

    if (!findEmptyPlace(row, col)) {

        bool unique_solution = true;

        // Check if the current board is already in solutions
        for (const auto& sol : solutions) {
            if (boardsEqual(sol, board)) {
                unique_solution = false;
                break;
            }
        }

        if (unique_solution) {
            solutions.push_back(board);
            printBoard(board);
        }
        return true;
    }

    for (int num = 1; num <= 9; num++) {
        if (isValid(row, col, num)) {
            board[row][col] = num;

            if (solveSudoku()) {
                return true;
            }

            board[row][col] = 0; // Turn to unassigned space when conditions are not satisfied
        }
    }
    
    return false;
}



// Generate initial Sudoku board
vector<vector<int>> initializeBoard() {
    vector<vector<int>> initialBoard(N, vector<int>(N, 0));
    srand(time(0));
    int count = NUM_STARTING_CELLS;

    while (count > 0) {
        int row = rand() % N;
        int col = rand() % N;
        int num = rand() % N + 1;

        if (initialBoard[row][col] == 0 && isValid(row, col, num)) {
            initialBoard[row][col] = num;
            count--;
        }
    }

    return initialBoard;
}

int main() {

    // board = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // };
    
    vector<vector<vector<int>>> solutions;

    board = initializeBoard();

    vector<vector<int> > start_board = board;
    printBoard(board);


    auto start_time = chrono::high_resolution_clock::now();
    
    for(int i=0; i<MAX; i++){
        // Reset flag before each call to solveSudoku
        std::cout<<"test";
        board = start_board;
        solveSudoku();
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    std::cout << "Number of solutions: " << solutions.size() << endl;
    std::cout << "Runtime duration: " << duration << " microseconds" << endl;

    // Print all solutions
    for (const auto& sol : solutions) {
        std::cout << "Solution:" << endl;
        printBoard(sol);
        std::cout << endl;
    }

    return 0;
}