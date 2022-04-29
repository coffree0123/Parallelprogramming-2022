#include <omp.h>
#include <stdio.h>

int zeros[100];
int total_zero = 0;
/* rowColConflict */
int rowColConflict(int try, int row, int col, int sudoku[9][9]) {
    int conflict = 0;
    for (int i = 0; i < 9 && !conflict; i++)
        if (((col != i) && (sudoku[row][i] == try)) ||
            ((row != i) && (sudoku[i][col] == try)))
            conflict = 1;
    return conflict;
}
/* blockConflict */
int blockConflict(int try, int row, int col, int sudoku[9][9]) {
    int blockRow = row / 3;
    int blockCol = col / 3;

    int conflict = 0;
    for (int i = 0; i < 3 && !conflict; i++)
        for (int j = 0; j < 3 && !conflict; j++)
            if (sudoku[3 * blockRow + i][3 * blockCol + j] == try) conflict = 1;
    return conflict;
}

/* conflict */
int conflict(int try, int row, int col, int sudoku[9][9]) {
    // check row conflict
    for (int i = 0; i < 9 && !conflict; i++)
        if (((col != i) && (sudoku[row][i] == try)) ||
            ((row != i) && (sudoku[i][col] == try)))
            return 1;

    // check col conflict
    int blockRow = row / 3;
    int blockCol = col / 3;

    for (int i = 0; i < 3 && !conflict; i++)
        for (int j = 0; j < 3 && !conflict; j++)
            if (sudoku[3 * blockRow + i][3 * blockCol + j] == try) return 1;

    return 0;
}
/* placeNumber */
int placeNumber(int n, int sudoku[9][9], int index) {
    if (index == total_zero) return 1;
    n = zeros[index];
    int row = n / 9;
    int col = n % 9;
    /* numSolution */
    int numSolution = 0;
    for (int try = 1; try <= 9; try++) {
        if (!conflict(try, row, col, sudoku)) {
            sudoku[row][col] = try;
            numSolution += placeNumber(n + 1, sudoku, index + 1);
        }
    } /* for */
    sudoku[row][col] = 0;
    return numSolution;
}

/* main */
int main(void) {
    int sudoku[9][9];
    int firstZero = -1;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            scanf("%d", &(sudoku[i][j]));
            if (sudoku[i][j] == 0 && firstZero == -1) firstZero = i * 9 + j;
            if (sudoku[i][j] == 0) {
                zeros[total_zero] = i * 9 + j;
                total_zero += 1;
            }
        }

    int numSolution = 0;
#pragma omp parallel for collapse(5) reduction(+ : numSolution) firstprivate(sudoku) schedule(dynamic, 1)
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            for (int k = 1; k <= 9; k++) {
                for (int t = 1; t <= 9; t++) {
                    for (int u = 1; u <= 9; u++) {
                        if (!conflict(i, zeros[0] / 9, zeros[0] % 9, sudoku) &&
                            !conflict(j, zeros[1] / 9, zeros[1] % 9, sudoku) &&
                            !conflict(k, zeros[2] / 9, zeros[2] % 9, sudoku) &&
                            !conflict(t, zeros[3] / 9, zeros[3] % 9, sudoku) &&
                            !conflict(u, zeros[4] / 9, zeros[4] % 9, sudoku)) {
                            sudoku[zeros[0] / 9][zeros[0] % 9] = i;
                            sudoku[zeros[1] / 9][zeros[1] % 9] = j;
                            sudoku[zeros[2] / 9][zeros[2] % 9] = k;
                            sudoku[zeros[3] / 9][zeros[3] % 9] = t;
                            sudoku[zeros[4] / 9][zeros[4] % 9] = u;
                            numSolution += placeNumber(firstZero, sudoku, 5);
                        }
                        sudoku[zeros[0] / 9][zeros[0] % 9] = 0;
                        sudoku[zeros[1] / 9][zeros[1] % 9] = 0;
                        sudoku[zeros[2] / 9][zeros[2] % 9] = 0;
                        sudoku[zeros[3] / 9][zeros[3] % 9] = 0;
                        sudoku[zeros[4] / 9][zeros[4] % 9] = 0;
                    }
                }
            }
        }
    }
    printf("%d\n", numSolution);
    return 0;
}
/* end */