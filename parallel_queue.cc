#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <cstdlib>
#include <pthread.h>
#include<atomic>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20;

pthread_mutex_t mtx; // Mutex for synchronization
vector<vector<int> > grid(N, vector<int>(N, 0));
queue<vector<vector<int>>> puzzle_list; // Global puzzle list
vector<vector<int>> final_board;

std::atomic<int> correct{0};
std::atomic<int> incorrect{0};

bool isValid(const vector<vector<int>>& board, int row, int col, int num) {
    // Check row and column
    for (int i = 0; i < N; ++i) {
        if (board[row][i] == num || board[i][col] == num)
            return false;
    }

    // Check 3x3 subgrid
    int startRow = row - row % SUBGRID_SIZE;
    int startCol = col - col % SUBGRID_SIZE;

    for (int i = 0; i < SUBGRID_SIZE; ++i) {
        for (int j = 0; j < SUBGRID_SIZE; ++j) {
            if (board[i + startRow][j + startCol] == num)
                return false;
        }
    }
    return true;
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

bool solveSudokuHelper(vector<vector<int>>& puzzle) {
    // Find an empty cell
    int row = -1;
    int col = -1;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (puzzle[i][j] == 0) {
                row = i;
                col = j;
                break;
            }
        }
        if (row != -1) break;
    }

    // If no empty cell found, the puzzle is solved
    if (row == -1) {
        return true;
    }
    vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    random_shuffle(nums.begin(), nums.end());

    // Try each number in the empty cell
    //for (int num = 1; num <= 9; ++num) {
    for(int num : nums){
        if (isValid(puzzle, row, col, num)) {
            // If the number is valid, update the puzzle with that number
            puzzle[row][col] = num;
            correct++;

            pthread_mutex_lock(&mtx);
            puzzle_list.push(puzzle);
            pthread_mutex_unlock(&mtx);
            
            // Recursively call solveSudokuHelper with the updated puzzle
            if (solveSudokuHelper(puzzle)) {
                return true; // If a solution was found, return true
            }

            // If solveSudokuHelper returns false, restore the original state of the puzzle
            puzzle[row][col] = 0;
            correct--;
            incorrect++;
        }
    }

    // If no solution was found, return false
    return false;
}

void* solveSudoku(void* arg) {
    while (true) {
        // Acquire lock before accessing the global puzzle list
        pthread_mutex_lock(&mtx);

        // Check if there are any puzzles left in the list
        if (puzzle_list.empty()) {
            // Release lock and terminate the thread if no more puzzles
            pthread_mutex_unlock(&mtx);
            break;
        }
        
        // Pop a puzzle from the global puzzle list
        vector<vector<int>> current_board = puzzle_list.front();
        puzzle_list.pop();
        
        // Release lock after accessing the global puzzle list
        pthread_mutex_unlock(&mtx);
        
        // Try to solve the Sudoku puzzle using backtracking
        if (solveSudokuHelper(current_board)) {
            // If a solution is found, update the final_board and clear the puzzle list
            pthread_mutex_lock(&mtx);
            final_board = current_board;
            puzzle_list = queue<vector<vector<int>>>();
            pthread_mutex_unlock(&mtx);
            break; // Break out of the loop if a solution is found
        }
    }
    
    return nullptr;
}


// Generate initial Sudoku board
void initializeBoard(vector<vector<int> >& board) {
    srand(time(0));
    int count = NUM_STARTING_CELLS;

    while (count > 0) {
        int row = rand() % N;
        int col = rand() % N;
        int num = rand() % N + 1;

        if (board[row][col] == 0 && isValid(board, row, col, num)) {
            board[row][col] = num;
            count--;
        }
    }
}

int main() {
   
    vector<vector<int> > board(N, vector<int>(N, 0));

    // ----- Empty Board ----- //
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

    vector<vector<int> > board(N, vector<int>(N, 0));

    //--EASY--
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

    //--MED--
    // board={{0, 0, 0, 4, 0, 5, 0, 0, 0},
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
    board={{0, 0, 0, 0, 0, 0, 2, 3, 0},
        {2, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 3, 0, 8, 0, 0},
        {8, 6, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 2, 9, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 0, 0, 7, 1, 0, 0, 0},
        {0, 0, 0, 0, 9, 3, 0, 0, 0},
        {0, 8, 0, 0, 0, 0, 0, 0, 0},
        };

    //initializeBoard(board);
    
    
    pthread_mutex_init(&mtx, nullptr);
    puzzle_list.push(board);

    // Create threads
    pthread_t thread1, thread2, thread3, thread4;// thread5, thread6, thread7, thread8;

    auto start_time = chrono::high_resolution_clock::now();

    pthread_create(&thread1, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread2, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread3, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread4, nullptr, solveSudoku, (void*)&board);
    // pthread_create(&thread5, nullptr, solveSudoku, (void*)&board);
    // pthread_create(&thread6, nullptr, solveSudoku, (void*)&board);
    // pthread_create(&thread7, nullptr, solveSudoku, (void*)&board);
    // pthread_create(&thread8, nullptr, solveSudoku, (void*)&board);


    // Wait for threads to finish
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    pthread_join(thread3, nullptr);
    pthread_join(thread4, nullptr);
    // pthread_join(thread5, nullptr);
    // pthread_join(thread6, nullptr);
    // pthread_join(thread7, nullptr);
    // pthread_join(thread8, nullptr);



    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    pthread_mutex_destroy(&mtx);

    cout << "Runtime duration: " << duration << " microseconds" << endl;
    std::cout << "Correct::" << correct << std::endl;
    std::cout << "Incorrect:: " << incorrect << std::endl;

    if (!final_board.empty()) {
        cout << "Solved successfully:\n";
        printBoard(final_board);
    } else {
        cout << "No solution exists\n";
    }

    return 0;
}