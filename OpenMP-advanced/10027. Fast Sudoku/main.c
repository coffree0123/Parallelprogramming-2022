#include <omp.h>
#include <stdio.h>

int zeroNum = 0;
int zeros[81];

int conflict(int try, int row, int col, int rowNum[9], int colNum[9],
             int blockNum[3][3]) {
    return (rowNum[row] & (1 << (try - 1)) || colNum[col] & (1 << (try - 1)) ||
            blockNum[row / 3][col / 3] & (1 << (try - 1)));
}

void on(int rowNum[9], int colNum[9], int blockNum[3][3], int i, int j,
        int num) {
    rowNum[i] |= (1 << (num - 1));
    colNum[j] |= (1 << (num - 1));
    blockNum[i / 3][j / 3] |= (1 << (num - 1));
}

void off(int rowNum[9], int colNum[9], int blockNum[3][3], int i, int j,
         int num) {
    rowNum[i] &= ~(1 << (num - 1));
    colNum[j] &= ~(1 << (num - 1));
    blockNum[i / 3][j / 3] &= ~(1 << (num - 1));
}

/* placeNumber */
int placeNumber(int n, int rowNum[9], int colNum[9], int blockNum[3][3]) {
    if (n == zeroNum) return 1;
    int row = zeros[n] / 9;
    int col = zeros[n] % 9;
    int numSolution = 0;

    for (int try = 1; try <= 9; try++) {
        if (!conflict(try, row, col, rowNum, colNum, blockNum)) {
            // sudoku[row][col] = try;
            on(rowNum, colNum, blockNum, row, col, try);
            numSolution += placeNumber(n + 1, rowNum, colNum, blockNum);
            off(rowNum, colNum, blockNum, row, col, try);
        }
    }
    return numSolution;
}

/* main */
int main(void) {
    // int sudoku[9][9];
    // rowNum[1] =
    int rowNum[9] = {0};
    int colNum[9] = {0};
    int blockNum[3][3] = {0};
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int num;
            scanf("%d", &num);
            if (num != 0) {
                on(rowNum, colNum, blockNum, i, j, num);
            } else {
                zeros[zeroNum] = i * 9 + j;
                zeroNum++;
            }
        }
    }

    int numSolution = 0;
#pragma omp parallel for reduction(+ : numSolution) firstprivate(rowNum, colNum, blockNum) collapse(4) schedule(dynamic, 1)
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            for (int k = 1; k <= 9; k++) {
                for (int l = 1; l <= 9; l++) {
                    if (!conflict(i, zeros[0] / 9, zeros[0] % 9, rowNum, colNum,
                                  blockNum) &&
                        !conflict(j, zeros[1] / 9, zeros[1] % 9, rowNum, colNum,
                                  blockNum) &&
                        !conflict(k, zeros[2] / 9, zeros[2] % 9, rowNum, colNum,
                                  blockNum) &&
                        !conflict(l, zeros[3] / 9, zeros[3] % 9, rowNum, colNum,
                                  blockNum)) {
                        on(rowNum, colNum, blockNum, zeros[0] / 9, zeros[0] % 9,
                           i);
                        on(rowNum, colNum, blockNum, zeros[1] / 9, zeros[1] % 9,
                           j);
                        on(rowNum, colNum, blockNum, zeros[2] / 9, zeros[2] % 9,
                           k);
                        on(rowNum, colNum, blockNum, zeros[3] / 9, zeros[3] % 9,
                           l);

                        numSolution += placeNumber(4, rowNum, colNum, blockNum);

                        off(rowNum, colNum, blockNum, zeros[3] / 9,
                            zeros[3] % 9, l);
                        off(rowNum, colNum, blockNum, zeros[2] / 9,
                            zeros[2] % 9, k);
                        off(rowNum, colNum, blockNum, zeros[1] / 9,
                            zeros[1] % 9, j);
                        off(rowNum, colNum, blockNum, zeros[0] / 9,
                            zeros[0] % 9, i);
                    }
                }
            }
        }
    }

    printf("%d\n", numSolution);
    return 0;
}
/* end */