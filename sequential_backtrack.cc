#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20;

vector<vector<int> > grid(N, vector<int>(N, 0));

int correct = 0;
int incorrect = 0;

//checks if num is in the column
bool isPresentCol(int col, int num){
    for (int row = 0; row < N; row++)
        if (grid[row][col] == num)
             return true;
    return false;
}

//checks if number is in the row
bool isPresentRow(int row, int num){
    for (int col = 0; col < N; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}

//checks if the number is in the local 3x3 grid
bool isPresentLocal(int boxStartRow, int boxStartCol, int num){
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i+boxStartRow][j+boxStartCol] == num)
                return true;
    return false;
}

//checks the row, column and local box
bool isValidPlace(int row, int col, int num){
   return !isPresentRow(row, num) && !isPresentCol(col, num) && !isPresentLocal(row - row%3 ,col - col%3, num);
}

// Generate initial Sudoku board
void initializeBoard(vector<vector<int> >& board) {
    srand(time(0));
    int count = NUM_STARTING_CELLS;

    while (count > 0) {
        int row = rand() % N;
        int col = rand() % N;
        int num = rand() % N + 1;

        if (board[row][col] == 0 && isValidPlace(row, col, num)) {
            board[row][col] = num;
            count--;
        }
    }
}

//find the first empty location in the grid
bool findEmptyPlace(int &row, int &col){
   for (row = 0; row < N; row++)
      for (col = 0; col < N; col++)
         if (grid[row][col] == 0) //marked with 0 is empty
            return true;
   return false;
}

//solver
bool solveSudoku(){
    int row, col;
    if (!findEmptyPlace(row, col))
        return true; //when all places are filled
    for (int num = 1; num <= 9; num++){ 
        //check if location is valid for num, then place it
        if (isValidPlace(row, col, num)){ 
            grid[row][col] = num;
            correct ++;
            if (solveSudoku()) //recursively go for other rooms in the grid
                return true;
            else {
                grid[row][col] = 0; //turn to unassigned space when conditions are not satisfied
                correct --;
                incorrect++;
            }
            
        }
    }
    return false;
}

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

int main(){
    //need to construct a grid or read one in from file
    //--EASY--
    // grid = {
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

    //--MED--
    // grid={{0, 0, 0, 4, 0, 5, 0, 0, 0},
    //     {5, 0, 0, 0, 2, 0, 0, 0, 1},
    //     {0, 0, 4, 0, 7, 0, 0, 0, 3},
    //     {0, 0, 8, 9, 4, 0, 6, 0, 0},
    //     {0, 0, 0, 8, 0, 0, 4, 0, 0},
    //     {1, 0, 0, 0, 6, 0, 0, 9, 0},
    //     {8, 0, 0, 3, 0, 0, 0, 0, 5},
    //     {0, 0, 9, 0, 0, 0, 0, 2, 0},
    //     {0, 0, 0, 0, 0, 8, 0, 0, 0},
    //     };

    //--HARD--
    // grid={{0, 0, 0, 0, 0, 0, 2, 3, 0},
    //     {2, 0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 3, 0, 8, 0, 0},
    //     {8, 6, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 2, 9, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 3, 0, 0, 7, 1, 0, 0, 0},
    //     {0, 0, 0, 0, 9, 3, 0, 0, 0},
    //     {0, 8, 0, 0, 0, 0, 0, 0, 0},
    //     };

    //--EMPTY--
    grid={{0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        };

    auto start_time = std::chrono::high_resolution_clock::now();
    bool res = solveSudoku();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "\nruntime duration:: " << duration << " microseconds\n" << std::endl;

    if (res){
        cout << "solution found! " << endl;
        cout << "Correct placements:: " << correct << endl;
        cout << "Incorrect placements:: " << incorrect << endl;

        printBoard();
    }     
    else
        cout << "No solution exists" << endl;
}