#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
using namespace std;
 
// N is the size of the 2D matrix   N*N
//#define N 9

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20;

//check if we can assign a number in grid
bool isSafe(vector<vector<int> >& grid, int row, int col, int num){
    // Check row
    for (int x = 0; x <= 8; x++)
        if (grid[row][x] == num)
            return false;
 
    // Check column
    for (int x = 0; x <= 8; x++)
        if (grid[x][col] == num)
            return false;
 
    // Check local 3x3 grid
    int startRow = row - row % SUBGRID_SIZE;
    int startCol = col - col % SUBGRID_SIZE;
   
    for (int i = 0; i < SUBGRID_SIZE; i++)
        for (int j = 0; j < SUBGRID_SIZE; j++)
            if (grid[i + startRow][j + startCol] == num)
                return false;
 
    return true;
}

// Generate initial Sudoku board
void initializeBoard(vector<vector<int> >& board) {
    srand(time(0));
    int count = NUM_STARTING_CELLS;

    while (count > 0) {
        int row = rand() % N;
        int col = rand() % N;
        int num = rand() % N + 1;

        if (board[row][col] == 0 && isSafe(board, row, col, num)) {
            board[row][col] = num;
            count--;
        }
    }
}

bool solveSudoku(vector<vector<int> >& grid, int row, int col){
    //reached end of matrix
    if (row == N - 1 && col == N)
        return true;
    // Reached end of row
    if (col == N) {
        row++;
        col = 0;
    }
    //if this column contains a value, move on
    if (grid[row][col] > 0)
        return solveSudoku(grid, row, col + 1);
 
    for (int num = 1; num <= N; num++) 
    {
         
        //check if we can place the nunber
        if (isSafe(grid, row, col, num)) 
        {
            grid[row][col] = num;
           
            //  Checking for next possibility
            if (solveSudoku(grid, row, col + 1))
                return true;
        }
       
        // removing the value, since it didn't lead to a solution
        grid[row][col] = 0;
    }
    return false;
}

// Function to print the Sudoku board
void printBoard(const vector<vector<int> >& board) {
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

int main(){

    vector<vector<int> > grid(N, vector<int>(N, 0));

    // initializeBoard(grid);
    // printBoard(grid);

    

    grid = {
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



    auto start_time = std::chrono::high_resolution_clock::now();
    bool res = solveSudoku(grid, 0, 0);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "\nruntime duration:: " << duration << " microseconds\n" << std::endl;

    if (res){
        cout << "solution found!" << endl;
        printBoard(grid);
    }
    else
        cout << "no solution exists " << endl;
 
    return 0;
}