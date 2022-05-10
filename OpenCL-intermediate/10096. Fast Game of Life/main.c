/* header */
#include <CL/cl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
 
#define MAXK 10000
#define MAX_PLATFORM 1
#define MAXGPU 10
#define MAXN 2022
#define BLOCK 16
 
char A[MAXN][MAXN];
char B[MAXN][MAXN];
/* print */
void print(char D[MAXN][MAXN], int n) {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) printf("%d", D[i][j]);
        printf("\n");
    }
}
 
cl_kernel kernel;
cl_int status;
cl_command_queue commandQueue;
cl_context context;
 
void build_opencl() {
    cl_platform_id platform_id[MAX_PLATFORM];
    cl_uint platform_id_got;
    status = clGetPlatformIDs(MAX_PLATFORM, platform_id, &platform_id_got);
    cl_device_id GPU[MAXGPU];
    cl_uint GPU_id_got;
    status = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, MAXGPU, GPU,
                            &GPU_id_got);
    context = clCreateContext(NULL, 1, &GPU[0], NULL, NULL, &status);
    commandQueue =
        clCreateCommandQueueWithProperties(context, GPU[0], NULL, &status);
    FILE *kernelfp = fopen("game-of-life.cl", "r");
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
    cl_program program = clCreateProgramWithSource(
        context, 1, &constKernelSource, &kernelLength, &status);
    status = clBuildProgram(program, 1, &GPU[0], NULL, NULL, NULL);
    kernel = clCreateKernel(program, "game", &status);
}
 
/* main */
int main() {
    int N, generation, cell;
 
    scanf("%d %d", &N, &generation);
 
    char str[MAXN];
    for (int i = 1; i <= N; i++) {
        scanf("%s", str);
        for (int j = 1; j <= N; j++) {
            A[i][j] = (str[j - 1] == '1');
        }
    }
 
    build_opencl();
    cl_mem bufferA =
        clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                       MAXN * MAXN * sizeof(char), A, &status);
    cl_mem bufferB =
        clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                       MAXN * MAXN * sizeof(char), B, &status);
 
    int fake_N = ((N - 1) / BLOCK + 1) * BLOCK;
    size_t globalThreads[] = {fake_N, fake_N};
    size_t localThreads[] = {BLOCK, BLOCK};
    status = clSetKernelArg(kernel, 0, sizeof(cl_int), (void *)&N);
 
    for (int g = 0; g < generation; g++) {
        if (g % 2 == 0) {
            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufferA);
            status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufferB);
        } else {
            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufferB);
            status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufferA);
        }
        status =
            clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalThreads,
                                   localThreads, 0, NULL, NULL);
    }
    clEnqueueReadBuffer(commandQueue, bufferA, CL_TRUE, 0,
                        MAXN * MAXN * sizeof(char), A, 0, NULL, NULL);
    clEnqueueReadBuffer(commandQueue, bufferB, CL_TRUE, 0,
                        MAXN * MAXN * sizeof(char), B, 0, NULL, NULL);
 
    clReleaseMemObject(bufferA); /* buffers */
    clReleaseMemObject(bufferB);
 
    /* printcell */
    if (generation % 2 == 0)
        print(A, N);
    else
        print(B, N);
}
/* end */