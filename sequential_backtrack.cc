#include <iostream>
#include <chrono>
#define N 9
using namespace std;

int grid[N][N] = { { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
                       { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
                       { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
                       { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
                       { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
                       { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
                       { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
                       { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
                       { 0, 0, 5, 2, 0, 0, 3, 0, 0 } };


//prints the grid
void print(){
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++)
            cout << grid[i][j] << " ";
        cout << endl;
    }
}

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

//find the first empty location in the grid
bool findEmptyPlace(int &row, int &col){
   for (row = 0; row < N; row++)
      for (col = 0; col < N; col++)
         if (grid[row][col] == 0) //marked with 0 is empty
            return true;
   return false;
}

//checks the row, column and local box
bool isValidPlace(int row, int col, int num){
   return !isPresentRow(row, num) && !isPresentCol(col, num) && !isPresentLocal(row - row%3 ,col - col%3, num);
}

//solver
bool solveSudoku(){
    int row, col;
    if (!findEmptyPlace(row, col))
        return true; //when all places are filled
    for (int num = 1; num <= 9; num++){ //valid numbers are 1 - 9
        //check if location is vlaid for num, then place it
        if (isValidPlace(row, col, num)){ 
            grid[row][col] = num;
            if (solveSudoku()) //recursively go for other rooms in the grid
                return true;
            grid[row][col] = 0; //turn to unassigned space when conditions are not satisfied
        }
    }
    return false;
}
int main(){
    //need to construct a grid or read one in from file
    auto start_time = std::chrono::high_resolution_clock::now();
    bool res = solveSudoku();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "runtime duration:: " << duration << std::endl;
    if (res){
        cout << "solution found! " << endl;
        print();
    }     
    else
        cout << "No solution exists";
}