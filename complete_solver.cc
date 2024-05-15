#include <atomic>
#include <iostream>
#include <vector>
#include <chrono>
#include <mutex>
#include <cstdlib> 
#include <algorithm>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 30;

const int MAX = 500;
int iter = 0;


vector<vector<int> > board(N, vector<int>(N, 0));
vector<vector<vector<int>>> solutions;

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
    
    // Check if the number is already present in the subgrid
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

bool findEmptyPlace(int &row, int &col){
   for (row = 0; row < N; row++)
      for (col = 0; col < N; col++)
         if (board[row][col] == 0) 
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

    if (!findEmptyPlace(row, col)) {
        
        bool unique_solution = true;

        // Check if the current board is already in solutions
        for (const auto& sol : solutions) {
            if (boardsEqual(sol, board)) {
                unique_solution = false;
                break;
            }
        }

        // If we find a new solution to the board
        if (unique_solution) {
            solutions.push_back(board);
        }
        
        return true;
    }

    // Shuffle the order of numbers to try
    vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    random_shuffle(nums.begin(), nums.end());

    for (int num : nums) {

        if (isValid(row, col, num)) {
            board[row][col] = num;

            if (solveSudoku()) {
                return true;
            }

            board[row][col] = 0; 
        }
    }
    return false;
}


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
    
// ----- Empty -----
//     board = 
//    {{0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0},
//     };

// --------------- True Sudoku Boards --------------- // 

    // ----- Easy ----- //
    // board = {
    //     {5, 3, 0, 0, 7, 0, 0, 0, 0},
    //     {6, 0, 0, 1, 9, 5, 0, 0, 0},
    //     {0, 9, 8, 0, 0, 0, 0, 6, 0},
    //     {8, 0, 0, 0, 6, 0, 0, 0, 3},
    //     {4, 0, 0, 8, 0, 3, 0, 0, 1},
    //     {7, 0, 0, 0, 2, 0, 0, 0, 6},
    //     {0, 6, 0, 0, 0, 0, 2, 8, 0},
    //     {0, 0, 0, 4, 1, 9, 0, 0, 5},
    //     {0, 0, 0, 0, 8, 0, 0, 7, 9}
    // };

    // ----- Medium ----- //
//     board = 
//    {{0, 0, 0, 4, 0, 5, 0, 0, 0},
//     {5, 0, 0, 0, 2, 0, 0, 0, 1},
//     {0, 0, 4, 0, 7, 0, 0, 0, 3},
//     {0, 0, 8, 9, 4, 0, 6, 0, 0},
//     {0, 0, 0, 8, 0, 0, 4, 0, 0},
//     {1, 0, 0, 0, 6, 0, 0, 9, 0},
//     {8, 0, 0, 3, 0, 0, 0, 0, 5},
//     {0, 0, 9, 0, 0, 0, 0, 2, 0},
//     {0, 0, 0, 0, 0, 8, 0, 0, 0},
//     };

// ----- Hard ----- //
    board = 
   {{0, 0, 0, 0, 0, 0, 5, 6, 0},
    {1, 0, 0, 0, 0, 9, 0, 0, 0},
    {4, 0, 0, 8, 0, 0, 0, 0, 0},
    {0, 5, 2, 0, 0, 0, 0, 7, 0},
    {0, 6, 0, 9, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 7, 0, 0, 5, 0, 0, 0, 0},
    {9, 0, 0, 0, 0, 0, 3, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1},
    };

// -------------------------------------------------- //

// --------------- Multi-Solut Boards --------------- // 

    // ----- Easy ----- //
    // board = {
    //     {5, 3, 0, 0, 7, 0, 9, 0, 2},
    //     {6, 0, 0, 1, 0, 4, 0, 0, 0},
    //     {0, 9, 8, 0, 3, 0, 0, 6, 0},
    //     {8, 0, 0, 0, 6, 0, 0, 0, 3},
    //     {0, 6, 0, 0, 0, 0, 0, 2, 1},
    //     {0, 0, 0, 0, 2, 0, 0, 0, 0},
    //     {0, 0, 3, 0, 0, 0, 2, 8, 0},
    //     {0, 0, 0, 7, 4, 9, 0, 0, 5},
    //     {1, 0, 0, 0, 8, 0, 0, 7, 9}
    // };

       // ----- Medium ----- //
    // board = {
    //     {0, 0, 0, 1, 0, 0, 2, 3, 0},
    //     {2, 0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 3, 0, 8, 0, 9},
    //     {8, 6, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 2, 9, 5, 0, 0, 7, 0},
    //     {0, 0, 0, 0, 0, 8, 0, 0, 0},
    //     {0, 3, 0, 0, 0, 1, 0, 0, 5},
    //     {0, 0, 0, 7, 9, 3, 0, 0, 0},
    //     {0, 8, 0, 0, 0, 0, 0, 4, 0},
    // };

       // ----- Hard ----- //
//     board = {
//     {0, 0, 0, 1, 0, 0, 0, 0, 0},
//     {2, 0, 0, 0, 0, 0, 0, 1, 0},
//     {0, 0, 0, 0, 3, 0, 0, 0, 0},
//     {8, 4, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 2, 0, 5, 0, 0, 7, 0},
//     {0, 0, 0, 0, 0, 8, 0, 0, 0},
//     {0, 3, 0, 0, 0, 1, 0, 0, 5},
//     {0, 0, 0, 7, 0, 3, 0, 0, 0},
//     {0, 8, 0, 0, 0, 0, 0, 4, 0},
//     };


    //board = initializeBoard();

    vector<vector<int> > start_board = board;
    printBoard(board);

    auto start_time = chrono::high_resolution_clock::now();
    
    for(int i=0; i<MAX; i++){
        // Reset board before solving again
        solveSudoku();
        board = start_board;
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Number of solutions: " << solutions.size() << endl;
    std::cout << "Runtime duration: " << duration << " seconds" << endl;



    return 0;
}