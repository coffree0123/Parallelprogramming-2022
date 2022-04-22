#include <math.h>
#include <omp.h>
#include <stdio.h>
#define MAX_N 500

void input_matrix(int row, int col, int matrix[MAX_N][MAX_N]) {
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            scanf("%d", &matrix[r][c]);
        }
    }
}

void print_matrix(int matrix[MAX_N][MAX_N], int row, int col) {
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            printf("%d%s", matrix[r][c], (c == col - 1) ? "\n" : " ");
        }
    }
}

int calculate_diff(int A[MAX_N][MAX_N], int B[MAX_N][MAX_N], int i, int j,
                   int A_row, int A_col, int B_row, int B_col) {
    int diff = 0;
    for (int r = 0; r < B_row; r++) {
        for (int c = 0; c < B_col; c++) {
            int v = (A[r + i][c + j] - B[r][c]);
            diff += v * v;
        }
    }

    return diff;
}

int main() {
    int A[MAX_N][MAX_N];
    int B[MAX_N][MAX_N];

    int A_row, A_col, B_row, B_col;
    while (scanf("%d %d %d %d", &A_row, &A_col, &B_row, &B_col) == 4) {
        input_matrix(A_row, A_col, A);
        input_matrix(B_row, B_col, B);
#ifdef DEBUG
        printf("A:\n");
        print_matrix(A, A_row, A_col);
        printf("B:\n");
        print_matrix(B, B_row, B_col);
#endif
        int diff[MAX_N][MAX_N];
        omp_set_num_threads(16);

#pragma omp parallel for
        for (int i = 0; i < A_row - B_row + 1; i++) {
            for (int j = 0; j < A_col - B_col + 1; j++) {
                diff[i][j] =
                    calculate_diff(A, B, i, j, A_row, A_col, B_row, B_col);
            }
        }

        int min = -1;
        int min_x, min_y;
        for (int i = 0; i < A_row - B_row + 1; i++) {
            for (int j = 0; j < A_col - B_col + 1; j++) {
                if (diff[i][j] < min || min == -1) {
                    min = diff[i][j];
                    min_x = i;
                    min_y = j;
                }
            }
        }
#ifdef DEBUG
        printf("Min:%d\n", min);
#endif
        printf("%d %d\n", min_x + 1, min_y + 1);
    }

    return 0;
}