#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20;

vector<vector<int>> grid(N, vector<int>(N, 0));
mutex mtx;

// Function to print the Sudoku board
void printBoard() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << grid[i][j] << " ";
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

//checks if num is in the column
bool isPresentCol(int col, int num) {
    for (int row = 0; row < N; row++)
        if (grid[row][col] == num)
            return true;
    return false;
}

//checks if number is in the row
bool isPresentRow(int row, int num) {
    for (int col = 0; col < N; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}

//checks if the number is in the local 3x3 grid
bool isPresentLocal(int boxStartRow, int boxStartCol, int num) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + boxStartRow][j + boxStartCol] == num)
                return true;
    return false;
}

//checks the row, column and local box
bool isValidPlace(int row, int col, int num) {
    return !isPresentRow(row, num) && !isPresentCol(col, num) && !isPresentLocal(row - row % 3, col - col % 3, num);
}

//find the first empty location in the grid
bool findEmptyPlace(int& row, int& col) {
    for (row = 0; row < N; row++)
        for (col = 0; col < N; col++)
            if (grid[row][col] == 0) //marked with 0 is empty
                return true;
    return false;
}

//solver
bool solveSudoku(int row, int col) {
    //int row, col;
    if (!findEmptyPlace(row, col)) {
        // All places are filled
        mtx.unlock(); // Release the lock
        return true;;
    }

    for (int num = 1; num <= 9; num++) { // Valid numbers are 1 - 9
       
        if (isValidPlace(row, col, num)) {\
            mtx.lock(); // Lock the mutex before modifying the grid
            grid[row][col] = num;
            mtx.unlock(); // Unlock the mutex after modifying the grid
            if (solveSudoku(row, col+1))
                return true; // Recursively go for other rooms in the grid
            else{
                mtx.lock(); // Lock the mutex again before backtracking
                grid[row][col] = 0; 
                mtx.unlock();
            }
            // Turn to unassigned space when conditions are not satisfied
        }
        mtx.unlock(); // Unlock the mutex after backtracking
    }
    return false;
}

int main() {
    // Initialize Sudoku board
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

    bool res = false;
    std::vector<std::thread> threads;
    auto start_time = chrono::high_resolution_clock::now();
    //mtx.lock(); // Lock the grid before starting the solver
    threads.emplace_back([&](){
                    res = res | solveSudoku(0, 0);
                });
    // threads.emplace_back([&](){
    //     res = res | solveSudoku(1, 1);
    // });
    threads.emplace_back([&](){
                    res = res | solveSudoku(3, 1);
                });
    threads.emplace_back([&](){
                    res = res | solveSudoku(7, 2);
                });

    for (auto &t : threads){
        t.join();
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    cout << "\nRuntime duration: " << duration << " microseconds\n" << endl;

    if (res){
        cout << "Solution: " << endl;
        printBoard();
    }
    else{
        cout << "no solution" << endl;
    }
    

    return 0;
}
