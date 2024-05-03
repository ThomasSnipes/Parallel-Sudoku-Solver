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
std::atomic<int> filled_counter{};
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
    //At end of grid 
    //also want to check that the grid is full
    // if (row <=2)
    //     std::cout << "operation on r,c: " << row << ", " << col << std::endl;
    if (row == N - 1 && col == N){
        // if (filled_counter == (N*N)){
        //     std::cout << "sat" << std::endl;
        //     return true;
        // }
        //then we are not done i.e the grid is not full. Send the worker thread back to the start
        // row = 0; 
        // col = 0;
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
            //scope for lock
            {
                lock_guard<mutex> lock(mtx);
                board[row][col] = num;
                filled_counter++;
                //std::cout << filled_counter << std::endl;
            }
            

            if (solveSudokuUtil(row, col + 1)) //increase column index here
                return true;

            //begin backtrack
            {
                lock_guard<mutex> lock(mtx);
                board[row][col] = 0;
                //bad choice, remove it
                filled_counter--;
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
                    res = res | solveSudokuUtil(0, N-1);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(N-1, 0);
                });
    threads.emplace_back([&](){
                    res = res | solveSudokuUtil(N-1, N-1);
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
void initializeBoard(vector<vector<int> >& board) {
    srand(time(0));
    int count = NUM_STARTING_CELLS;

    while (count > 0) {
        int row = rand() % N;
        int col = rand() % N;
        int num = rand() % N + 1;

        if (board[row][col] == 0 && isValid(row, col, num)) {
            board[row][col] = num;
            count--;
        }
    }
}

int main() {


    // board =  {{0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // {0, 0, 0, 0, 0, 0, 0, 0, 0},
    // };

    initializeBoard(board);
    //printBoard(board);
    if (solveSudoku()) {
        cout << "Sudoku solved successfully:\n";
        printBoard(board);
    } else {
        cout << "No solution exists for the given Sudoku.\n";
    }
    return 0;
}