#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int num_threads = 4;
//const int NUM_STARTING_CELLS = 20;

vector<vector<int> > board(N, vector<int>(N, 0));

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

mutex mtx; //global mutex

bool solveSudokuUtil(int row, int col) {
    if (row == N - 1 && col == N){
        return true;
    }
    // Move to the next row if the column index has reached the end
    if (col == N) {
        row++;
        col = 0;
    }
    //move to next cell if current is occupied
    if (board[row][col] != 0)
        return solveSudokuUtil(row, col + 1);

    //range of possible numbers is 1-9
    for (int num = 1; num <= N; ++num) {
        // Check if it's safe to place 'num' in the current cell
        if (isValid(row, col, num)) {
            // Place the number and recursively solve for the next cell
            //lock before placing
            {//scope for lock
                lock_guard<mutex> lock(mtx);
                board[row][col] = num;
            }
            if (solveSudokuUtil(row, col + 1)) //increase column index here
                return true;
            {//placement was bad
                lock_guard<mutex> lock(mtx);
                board[row][col] = 0;
            }
        }
    }
    return false; // No solution found
}

bool solveSudokuUtil1(int row, int col) {
    if (row == N - 1 && col == -1){
        return true;
    }
    // Move to the next row if the column index has reached the end
    if (col == -1) {
        row++;
        col = N-1;
    }
    //move to next cell if current is occupied
    if (board[row][col] != 0)
        return solveSudokuUtil1(row, col - 1);

    //range of possible numbers is 1-9
    for (int num = 1; num <= N; ++num) {
        // Check if it's safe to place 'num' in the current cell
        if (isValid(row, col, num)) {
            // Place the number and recursively solve for the next cell
            //lock before placing
            {//scope for lock
                lock_guard<mutex> lock(mtx);
                board[row][col] = num;
            }
            if (solveSudokuUtil1(row, col - 1)) //increase column index here
                return true;
            {//placement was bad
                lock_guard<mutex> lock(mtx);
                board[row][col] = 0;
            }
        }
    }
    return false; // No solution found
}

bool solveSudokuUtil2(int row, int col) {
    if (row == 0 && col == -1){
        return true;
    }
    // Move to the next row if the column index has reached the end
    if (col == -1) {
        row--;
        col = 0;
    }
    //move to next cell if current is occupied
    if (board[row][col] != 0)
        return solveSudokuUtil2(row, col - 1);

    //range of possible numbers is 1-9
    for (int num = 1; num <= N; ++num) {
        // Check if it's safe to place 'num' in the current cell
        if (isValid(row, col, num)) {
            // Place the number and recursively solve for the next cell
            //lock before placing
            {//scope for lock
                lock_guard<mutex> lock(mtx);
                board[row][col] = num;
            }
            if (solveSudokuUtil2(row, col - 1)) //increase column index here
                return true;
            {//placement was bad
                lock_guard<mutex> lock(mtx);
                board[row][col] = 0;
            }
        }
    }
    return false; // No solution found
}

bool solveSudokuUtil3(int row, int col) {
    if (row == 0 && col == N){
        return true;
    }
    // Move to the next row if the column index has reached the end
    if (col == N) {
        row--;
        col = 0;
    }
    //move to next cell if current is occupied
    if (board[row][col] != 0)
        return solveSudokuUtil3(row, col + 1);

    //range of possible numbers is 1-9
    for (int num = 1; num <= N; ++num) {
        // Check if it's safe to place 'num' in the current cell
        if (isValid(row, col, num)) {
            // Place the number and recursively solve for the next cell
            //lock before placing
            {//scope for lock
                lock_guard<mutex> lock(mtx);
                board[row][col] = num;
            }
            if (solveSudokuUtil3(row, col + 1)) //increase column index here
                return true;
            {//placement was bad
                lock_guard<mutex> lock(mtx);
                board[row][col] = 0;
            }
        }
    }
    return false; // No solution found
}

bool solveSudoku() {

    std::vector<thread> threads;
    bool res = false;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(0, 0);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil1(0, N-1);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil2(N-1, 0);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil3(N-1, N-1);
                });
    for (auto &t : threads){
        t.join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "runtime duration:: " << duration << " microseconds" << std::endl;
    return res;
}

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

// Generate initial Sudoku board
// void initializeBoard(vector<vector<int> >& board) {
//     srand(time(0));
//     int count = NUM_STARTING_CELLS;

//     while (count > 0) {
//         int row = rand() % N;
//         int col = rand() % N;
//         int num = rand() % N + 1;

//         if (board[row][col] == 0 && isValid(row, col, num)) {
//             board[row][col] = num;
//             count--;
//         }
//     }
// }

int main() {


    // ----- Empty Board ----- //
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

    //initializeBoard(board);
    //printBoard(board);
    if (solveSudoku()) {
        cout << "Sudoku solved successfully:\n";
        printBoard(board);
    } else {
        cout << "No solution exists for the given Sudoku.\n";
    }
    return 0;
}