#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <shared_mutex>

using namespace std;

const int N = 9;
const int subSize = 3;
const int num_threads = 5;
std::atomic<int> filled_counter{0};
std::atomic<int> correct{0};
std::atomic<int> incorrect{0};
vector<vector<int>> board;

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
    for (int i = 0; i < subSize; ++i) {
        for (int j = 0; j < subSize; ++j) {
            if (board[i + startRow][j + startCol] == num)
                return false;
        }
    }
    return true;
}



bool solveSudokuUtil(int row, int col) {
    // At end of grid 
    // also want to check that the grid is full
    
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
            // lock before placing
            // scope for lock
            {
                lock_guard<mutex> lock(mtx);
                board[row][col] = num;
                filled_counter++;
                correct++;
            }
            

            if (solveSudokuUtil(row, col + 1)) //increase column index here
                return true;

            //begin backtrack
            {
                lock_guard<mutex> lock(mtx);
                board[row][col] = 0;

                //bad choice, remove it
                filled_counter--;
                correct --;
                incorrect ++;
            }
        }
    }
    return false; // No solution found
}

bool solveSudoku() {
    //measure performance
    //spawn threads here - start at 4 locations in the board
    std::vector<thread> threads;
    bool res = false;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // for (int i = 0; i < num_threads; i++){
    //     threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(i, 0);
    //             });

    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(0, 0);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(2, 2);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(4, 4);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(6, 2);
                });

    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(0, 0);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(2, 2);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(4, 4);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(6, 7);
    //             });

    //--5 THREAD SET--
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(0, 0);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(1, 7);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(3, 5);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(5, 3);
    //             });
    // threads.emplace_back([&](){
    //                 res = res | solveSudokuUtil(7, 1);
    //             });


    
    for (auto &t : threads){
        t.join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "runtime duration:: " << duration << " microseconds" << std::endl;
    std::cout << "Filled::: " << filled_counter << std::endl;
    std::cout << "Correct::" << correct << std::endl;
    std::cout << "Incorrect:: " << incorrect << std::endl;
    return res;
}

void printBoard() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {

    // ----- Empty -----
    board = 
   {{0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

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
    //     board = 
    //    {{0, 0, 0, 0, 0, 0, 5, 6, 0},
    //     {1, 0, 0, 0, 0, 9, 0, 0, 0},
    //     {4, 0, 0, 8, 0, 0, 0, 0, 0},
    //     {0, 5, 2, 0, 0, 0, 0, 7, 0},
    //     {0, 6, 0, 9, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 1, 0, 0, 0, 0, 0},
    //     {0, 7, 0, 0, 5, 0, 0, 0, 0},
    //     {9, 0, 0, 0, 0, 0, 3, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0, 1},
    //     };

    if (solveSudoku()) {
        cout << "Sudoku solved successfully:\n";
        printBoard();
    } else {
        cout << "No solution exists for the given Sudoku.\n";
    }
    return 0;

}