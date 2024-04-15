#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>

using namespace std;

const int N = 9;
const int SUBGRID_SIZE = 3;
const int NUM_STARTING_CELLS = 20;

mutex mtx; // Mutex for synchronization
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

void printPuzzleList(const queue<vector<vector<int>>>& puzzle_list) {
    // Make a copy of the puzzle list to avoid modifying the original
    queue<vector<vector<int>>> copy = puzzle_list;

    // Iterate through each puzzle in the queue
    while (!copy.empty()) {
        // Get the current puzzle
        vector<vector<int>> current_board = copy.front();
        copy.pop();

        // Print the puzzle
        for (const auto& row : current_board) {
            for (int cell : row) {
                cout << cell << " ";
            }
            cout << endl;
        }
        cout << endl; // Add a blank line between puzzles
    }
}
void solveSudoku(vector<vector<int>> board) {
    mtx.lock();
    puzzle_list.push(board);
    mtx.unlock();

    while (!puzzle_list.empty()) {

        mtx.lock();
        vector<vector<int>> current_board = puzzle_list.front();
        puzzle_list.pop();
        mtx.unlock();
       // printBoard(current_board);
        //std::cout << "\n";
        bool found_solution = false;

        // Find an empty cell
        int row = -1;
        int col = -1;

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (current_board[i][j] == 0) {
                    row = i;
                    col = j;
                    break;
                }
            }
            if (row != -1) break;
        }

        // If no empty cell found, we have a solution
        if (row == -1) {
            found_solution = true;
            mtx.lock();
            //printPuzzleList(puzzle_list);
            final_board = current_board;
            puzzle_list = queue<vector<vector<int>>>(); // Clear puzzle list to signal other threads to terminate

            mtx.unlock();
        }

        if (found_solution) break;

        // Try each number in the empty cell
        for (int num = 1; num <= 9; ++num) {
            if (isValid(current_board, row, col, num)) {
                current_board[row][col] = num;

                // Push the new board to the puzzle list
                mtx.lock();
                puzzle_list.push(current_board);
                mtx.unlock();
            }
        }
        
    }
    
}




int main() {
    vector<vector<int>> board = {
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

    auto start_time = chrono::high_resolution_clock::now();

    thread t1(solveSudoku, board);
    thread t2(solveSudoku, board);
    
    t1.join();
    t2.join();
    printBoard(board);
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
