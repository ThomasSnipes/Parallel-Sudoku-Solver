#include <iostream>
 
using namespace std;
 
// N is the size of the 2D matrix   N*N
#define N 9
 
void print(int arr[N][N]){
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++)
            cout << arr[i][j] << " ";
        cout << endl;
    }
}
 
//check if we can assign a number in grid
bool isSafe(int grid[N][N], int row, int col, int num){
    // Check row
    for (int x = 0; x <= 8; x++)
        if (grid[row][x] == num)
            return false;
 
    // Check column
    for (int x = 0; x <= 8; x++)
        if (grid[x][col] == num)
            return false;
 
    // Check local 3x3 grid
    int startRow = row - row % 3, 
            startCol = col - col % 3;
   
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j + 
                            startCol] == num)
                return false;
 
    return true;
}
 
bool solveSudoku(int grid[N][N], int row, int col){
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
 
int main(){
    //test grid
    //change this later to read in a grid we generate?
    int grid[N][N] = { { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
                       { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
                       { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
                       { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
                       { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
                       { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
                       { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
                       { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
                       { 0, 0, 5, 2, 0, 0, 3, 0, 0 } };
 
    if (solveSudoku(grid, 0, 0)){
        cout << "solution found!" << endl;
        print(grid);
    }
    else
        cout << "no solution  exists " << endl;
 
    return 0;
}