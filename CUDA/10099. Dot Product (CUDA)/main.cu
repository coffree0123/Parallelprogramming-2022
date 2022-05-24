#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <inttypes.h>
#include <stdint.h>
#define MAXN 16777216
#define BlockSize 512
 
__device__ uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
__device__ uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}
 
__global__ void vecDot(uint32_t int_array[MAXN/BlockSize], uint32_t key1, uint32_t key2, int N)
{
    uint32_t sum = 0;
    int start = blockIdx.x * BlockSize;
    for (int i = start; i < N && i < start + BlockSize; i++)
        sum += encrypt(i, key1) * encrypt(i, key2);
 
    int_array[blockIdx.x] = sum;
}
 
uint32_t A[MAXN/BlockSize], B[MAXN/BlockSize], C[MAXN/BlockSize];
int main(int argc, char *argv[]) {
    int N;
    uint32_t *device_array;
    uint32_t key1, key2;
    cudaMalloc((void **)&device_array, (MAXN / BlockSize) * sizeof(uint32_t));
 
    while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
        cudaMemcpy(device_array, A, (MAXN / BlockSize) * sizeof(uint32_t), cudaMemcpyHostToDevice);
 
        int total_group = N / BlockSize + 1;
        if (total_group % BlockSize != 0)
            total_group = (total_group/BlockSize + 1) * BlockSize;
 
        vecDot <<< total_group, 1 >>> (device_array, key1, key2, N);
 
        cudaMemcpy(C, device_array, (MAXN / BlockSize) * sizeof(uint32_t), cudaMemcpyDeviceToHost);
 
        uint32_t sum = 0;
        for (int i = 0; i < total_group; i++)
            sum += C[i];
        printf("%" PRIu32 "\n", sum);
    }
    return 0;
}