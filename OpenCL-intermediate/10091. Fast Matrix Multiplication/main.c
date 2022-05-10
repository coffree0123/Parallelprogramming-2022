#include <CL/cl.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// #define DEBUG
#define UINT uint32_t
#define Blk 32
#define MAXN 1024
#define MAXGPU 10
#define MAXK 1024
 
void multiply(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
#ifdef Correct
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            UINT sum = 0;  // overflow, let it go.
            for (int k = 0; k < N; k++) sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
#else
    cl_int status;
    cl_platform_id platform_id;
    cl_uint platform_id_got;
    // Create platform
    status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
    assert(status == CL_SUCCESS && platform_id_got == 1);
    // Create device
    cl_device_id GPU[MAXGPU];
    cl_uint GPU_id_got;
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, MAXGPU, GPU,
                            &GPU_id_got);
    assert(status == CL_SUCCESS);
    /* getcontext */
    cl_context context =
        clCreateContext(NULL, GPU_id_got, GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
    /* commandqueue */
    cl_command_queue commandQueue =
        clCreateCommandQueueWithProperties(context, GPU[0], NULL, &status);
    assert(status == CL_SUCCESS);
    /* kernelsource */
    FILE *kernelfp = fopen("matrixmul.cl", "r");
    assert(kernelfp != NULL);
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
    cl_program program = clCreateProgramWithSource(
        context, 1, &constKernelSource, &kernelLength, &status);
    assert(status == CL_SUCCESS);
    /* buildprogram */
    status = clBuildProgram(program, GPU_id_got, GPU, NULL, NULL, NULL);
    if (status != CL_SUCCESS) {
        char programBuffer[MAXK];
        size_t log_length;
        status = clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, MAXK, programBuffer, &log_length);
        programBuffer[log_length] = '\0';
        printf("%s", programBuffer);
    }
    /* createkernel */
    cl_kernel kernel = clCreateKernel(program, "mul", &status);
    assert(status == CL_SUCCESS);
    /* createbuffer */
    cl_mem bufferA =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       MAXN * MAXN * sizeof(cl_uint), A, &status);
    assert(status == CL_SUCCESS);
    cl_mem bufferB =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       MAXN * MAXN * sizeof(cl_uint), B, &status);
    assert(status == CL_SUCCESS);
    cl_mem bufferC =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                       MAXN * MAXN * sizeof(cl_uint), C, &status);
    assert(status == CL_SUCCESS);
    /* setarg */
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bufferA);
    assert(status == CL_SUCCESS);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufferB);
    assert(status == CL_SUCCESS);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufferC);
    assert(status == CL_SUCCESS);
    status = clSetKernelArg(kernel, 3, sizeof(int), (void *)&N);
    assert(status == CL_SUCCESS);
    /* setshape */
    size_t globalThreads[] = {(size_t)N, (size_t)N};
    size_t localThreads[] = {Blk, Blk};
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL,
                                    globalThreads, localThreads, 0, NULL, NULL);
    assert(status == CL_SUCCESS);
    // wait
    // clFinish(commandQueue);
    clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 0,
                        MAXN * MAXN * sizeof(cl_uint), C, 0, NULL, NULL);
    // free memory
    clReleaseContext(context); /* context etcmake */
    clReleaseCommandQueue(commandQueue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(bufferA); /* buffers */
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
#endif
 
}
void rand_gen(UINT c, int N, UINT A[][MAXN]) {
    UINT x = 2, n = N * N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x = (x * x + c + i + j) % n;
            A[i][j] = x;
        }
    }
}
void print_matrix(int N, UINT A[][MAXN]) {
    for (int i = 0; i < N; i++) {
        fprintf(stderr, "[");
        for (int j = 0; j < N; j++) fprintf(stderr, " %u", A[i][j]);
        fprintf(stderr, " ]\n");
    }
    fprintf(stderr, "\n\n");
}
UINT signature(int N, UINT A[][MAXN]) {
    UINT h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) h = (h + A[i][j]) * 2654435761LU;
    }
    return h;
}
UINT A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
int main() {
    int N;
    uint32_t S1, S2;
    scanf("%d %u %u", &N, &S1, &S2);
    rand_gen(S1, N, A);
    rand_gen(S2, N, B);
 
    // Parallel on this line
    multiply(N, A, B, C);
 
#ifdef DEBUG
    print_matrix(N, A);
    print_matrix(N, B);
    print_matrix(N, C);
#endif
 
    printf("%u\n", signature(N, C));
    return 0;
}