/* header */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#define MAXN 2022
#define BLOCK 16
 
/* print */
void print(char D[MAXN][MAXN], int n) {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) printf("%d", D[i][j]);
        printf("\n");
    }
}
 
__global__ void game(int N, char A[MAXN][MAXN], char B[MAXN][MAXN]) {
    int nln = 0;
    int global_row = blockIdx.x * BLOCK + threadIdx.x + 1;
    int global_col = blockIdx.y * BLOCK + threadIdx.y + 1;
 
    int d[8][2] = {{-1, -1}, {-1, 0}, {0, -1}, {-1, 1},
                    {1, -1},  {0, 1},  {1, 0},  {1, 1}};
 
    if(global_row >= (N + 1) || global_col >= (N + 1)) return;
 
    for (int k = 0; k < 8; k++) {
        nln += A[global_row + d[k][0]][global_col + d[k][1]];
    }
    B[global_row][global_col] = ((A[global_row][global_col] == 0 && nln == 3) || (A[global_row][global_col] == 1 && (nln == 2 || nln == 3)));
 
}
 
char A[MAXN][MAXN];
char B[MAXN][MAXN];
 
/* main */
int main() {
    int N, generation;
 
    char *device_A;
 
    char *device_B;
 
    cudaMalloc((void **)&device_A, MAXN * MAXN * sizeof(char));
 
    cudaMalloc((void **)&device_B, MAXN * MAXN * sizeof(char));
 
    scanf("%d %d", &N, &generation);
 
    char str[MAXN];
    for (int i = 1; i <= N; i++) {
        scanf("%s", str);
        for (int j = 1; j <= N; j++) {
            A[i][j] = (str[j - 1] == '1')? 1 :0;
        }
    }
 
    cudaMemcpy(device_A, A, MAXN * MAXN * sizeof(char), cudaMemcpyHostToDevice);
 
    cudaMemcpy(device_B, B, MAXN * MAXN * sizeof(char), cudaMemcpyHostToDevice);
 
    int fake_N = ((N + 1 + BLOCK) / BLOCK);
    dim3 grid(fake_N, fake_N);
    dim3 block(BLOCK, BLOCK);
 
    for (int g = 0; g < generation; g++) {
        // printf("%d\n\n", g+1);
        if (g % 2 == 0) {
            // From A to B
            game <<< grid, block >>> (N, (char (*)[MAXN])device_A, (char (*)[MAXN])device_B);
            // cudaMemcpy(B, device_B, MAXN * MAXN * sizeof(char), cudaMemcpyDeviceToHost);
            // print(B, N);
        } else {
            // From B to A
            game <<< grid, block >>> (N, (char (*)[MAXN])device_B, (char (*)[MAXN])device_A);
            // cudaMemcpy(A, device_A, MAXN * MAXN * sizeof(char), cudaMemcpyDeviceToHost);
            // print(A, N);
        }
    }
 
    /* printcell */
    if (generation % 2 == 0) {
        cudaMemcpy(A, device_A, MAXN * MAXN * sizeof(char), cudaMemcpyDeviceToHost);
        print(A, N);
    } else {
        cudaMemcpy(B, device_B, MAXN * MAXN * sizeof(char), cudaMemcpyDeviceToHost);
        print(B, N);
    }
}
/* end */