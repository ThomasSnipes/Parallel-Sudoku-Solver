#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <pthread.h>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20;

pthread_mutex_t mtx; // Mutex for synchronization
vector<vector<int> > grid(N, vector<int>(N, 0));
queue<vector<vector<int>>> puzzle_list; // Global puzzle list
vector<vector<int>> final_board;

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

    // Try each number in the empty cell
    for (int num = 1; num <= 9; ++num) {
        if (isValid(puzzle, row, col, num)) {
            // If the number is valid, update the puzzle with that number
            puzzle[row][col] = num;

            // Recursively call solveSudokuHelper with the updated puzzle
            if (solveSudokuHelper(puzzle)) {
                return true; // If a solution was found, return true
            }

            // If solveSudokuHelper returns false, restore the original state of the puzzle
            puzzle[row][col] = 0;
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
// void* solveSudoku(void* arg) {
//     //vector<vector<int>>* board = static_cast<vector<vector<int>>*>(arg);

//     //vector<vector<int>> current_board = *board;

//     while (true) {
        
//         // Acquire lock before accessing the global puzzle list
//         pthread_mutex_lock(&mtx);
//         //puzzle_list.push(*board);
//         //pthread_mutex_unlock(&mtx);

//         if (puzzle_list.empty()) {
//             // Release lock and terminate thread if no more puzzles
//             pthread_mutex_unlock(&mtx);
            
//             break;
//         }
        
//         // Pop a puzzle from the global puzzle list
//         vector<vector<int>> current_board = puzzle_list.front();
//         puzzle_list.pop();
        
//         // Release lock after accessing the global puzzle list
//         pthread_mutex_unlock(&mtx);
        
//         bool found_solution = false;

//         // Find an empty cell
//         int row = -1;
//         int col = -1;

//         for (int i = 0; i < N; ++i) {
//             for (int j = 0; j < N; ++j) {
//                 if (current_board[i][j] == 0) {
//                     row = i;
//                     col = j;
//                     break;
//                 }
//             }
//             if (row != -1) break;
//         }
        

//         // If no empty cell found, we have a solution
//         if (row == -1) {
//             found_solution = true;
            
//             // Acquire lock before updating shared resources
//             pthread_mutex_lock(&mtx);
            
//             // Update final_board if solution found
//             final_board = current_board;

//             // Clear puzzle list to signal other threads to terminate
//             puzzle_list = queue<vector<vector<int>>>();

//             // Release lock after updating shared resources
//             pthread_mutex_unlock(&mtx);
//         }

//         // If solution found, break out of the loop
//         if (found_solution) break;

//         // Try each number in the empty cell
//         for (int num = 1; num <= 9; ++num) {
//             if (isValid(current_board, row, col, num)) {

//                 current_board[row][col] = num;

//                 // Acquire lock before pushing the new board to the puzzle list
//                 pthread_mutex_lock(&mtx);

//                 // printBoard(current_board);
//                 // std::cout<<"\n";

//                 // Push the new board to the global puzzle list
//                 puzzle_list.push(current_board);
                
//                 // Release lock after pushing the new board
//                 pthread_mutex_unlock(&mtx);
//             }
//         }
//     }
    
//     return nullptr;
// }

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
    // vector<vector<int>> board = {
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

    // vector<vector<int>> board = {
    //     {0, 0, 0, 3, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 8, 0, 7, 0},
    //     {0, 0, 0, 9, 2, 0, 0, 4, 0},
    //     {0, 0, 2, 0, 0, 0, 0, 9, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 3, 0},
    //     {7, 3, 0, 6, 0, 0, 2, 0, 0},
    //     {0, 0, 8, 0, 0, 0, 6, 1, 9},
    //     {0, 0, 3, 0, 0, 5, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 6, 0, 0, 0}
    // };

    vector<vector<int> > board(N, vector<int>(N, 0));
    initializeBoard(board);
    
    printBoard(board);
    
    pthread_mutex_init(&mtx, nullptr);
    puzzle_list.push(board);

    // Create threads
    pthread_t thread1, thread2, thread3, thread4, thread5, thread6, thread7, thread8;

    auto start_time = chrono::high_resolution_clock::now();

    pthread_create(&thread1, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread2, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread3, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread4, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread5, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread6, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread7, nullptr, solveSudoku, (void*)&board);
    pthread_create(&thread8, nullptr, solveSudoku, (void*)&board);

    // Wait for threads to finish
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    pthread_join(thread3, nullptr);
    pthread_join(thread4, nullptr);
    pthread_join(thread5, nullptr);
    pthread_join(thread6, nullptr);
    pthread_join(thread7, nullptr);
    pthread_join(thread8, nullptr);

    pthread_mutex_destroy(&mtx);


    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    cout << "Runtime duration: " << duration << " microseconds" << endl;

    if (!final_board.empty()) {
        cout << "Solved successfully:\n";
        printBoard(final_board);
    } else {
        cout << "No solution exists\n";
    }

    return 0;
}


// void* solveSudoku(void* arg) {
//     vector<vector<int>>* board = static_cast<vector<vector<int>>*>(arg);

//     pthread_mutex_lock(&mtx);
//     puzzle_list.push(*board);
//     pthread_mutex_unlock(&mtx);

//     while (!puzzle_list.empty()) {

//         pthread_mutex_lock(&mtx);
//         vector<vector<int>> current_board = puzzle_list.front();
//         puzzle_list.pop();
//         pthread_mutex_unlock(&mtx);
//         bool found_solution = false;

//         // Find an empty cell
//         int row = -1;
//         int col = -1;

//         for (int i = 0; i < N; ++i) {
//             for (int j = 0; j < N; ++j) {
//                 if (current_board[i][j] == 0) {
//                     row = i;
//                     col = j;
//                     break;
//                 }
//             }
//             if (row != -1) break;
//         }

//         // If no empty cell found, we have a solution
//         if (row == -1) {
//             found_solution = true;
//             pthread_mutex_lock(&mtx);
//             final_board = current_board;
//             puzzle_list = queue<vector<vector<int>>>(); // Clear puzzle list to signal other threads to terminate
//             pthread_mutex_unlock(&mtx);
//         }

//         if (found_solution) break;

//         // Try each number in the empty cell
//         for (int num = 1; num <= 9; ++num) {
//             if (isValid(current_board, row, col, num)) {
//                 current_board[row][col] = num;

//                 // Push the new board to the puzzle list
//                 pthread_mutex_lock(&mtx);
//                 puzzle_list.push(current_board);
//                 pthread_mutex_unlock(&mtx);
//             }
//         }
        
//     }
//     return nullptr;
//  }