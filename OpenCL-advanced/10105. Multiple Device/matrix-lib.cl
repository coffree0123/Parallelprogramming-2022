#define MAXN 1024
#define uint32_t unsigned int
 
__kernel void mul(__global int matrixA[MAXN][MAXN],
            __global int matrixB[MAXN][MAXN],
            __global int matrixC[MAXN][MAXN], int N)
{
    int row = get_global_id(0);
    int col = get_global_id(1);
    int sum = 0;
    for (int i = 0; i < N; i++)
        sum += matrixA[row][i] * matrixB[i][col];
    matrixC[row][col] = sum;
}