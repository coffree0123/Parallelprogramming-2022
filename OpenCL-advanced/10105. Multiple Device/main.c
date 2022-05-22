#include <CL/cl.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// #define DEBUG
#define UINT uint32_t
#define Blk 1
#define MAXN 1024
#define MAXK 4096
 
UINT IN[6][MAXN][MAXN], TMP[6][MAXN][MAXN];
 
int MAXGPU = 2;
cl_int status;
cl_platform_id platform_id;
cl_uint platform_id_got;
cl_device_id GPU[6];
cl_uint GPU_id_got;
cl_context context;
cl_command_queue commandQueue[6];
cl_program program;
cl_kernel kernel;
 
void multiply(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN], int device) {
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
    int fake_N = ((N - 1) / Blk + 1) * Blk;
    size_t globalThreads[] = {(size_t)fake_N, (size_t)fake_N};
    size_t localThreads[] = {Blk, Blk};
    status = clEnqueueNDRangeKernel(commandQueue[device], kernel, 2, NULL,
                                    globalThreads, localThreads, 0, NULL, NULL);
    assert(status == CL_SUCCESS);
    clEnqueueReadBuffer(commandQueue[device], bufferC, CL_TRUE, 0,
                        MAXN * MAXN * sizeof(cl_uint), C, 0, NULL, NULL);
}
 
void add(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
#pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) C[i][j] = A[i][j] + B[i][j];
    }
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
}
 
UINT signature(int N, UINT A[][MAXN]) {
    UINT h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) h = (h + A[i][j]) * 2654435761LU;
    }
    return h;
}
 
void build_opencl() {
    // Create platform
    status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
    assert(status == CL_SUCCESS && platform_id_got == 1);
    // Create device
 
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, MAXGPU, GPU,
                            &GPU_id_got);
    MAXGPU = (GPU_id_got > 1)? 2 : 1;
    assert(status == CL_SUCCESS);
    /* getcontext */
    context =
        clCreateContext(NULL, MAXGPU, GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
    /* commandqueue */
    for (int i = 0; i < MAXGPU; i++) {
        commandQueue[i] =
            clCreateCommandQueueWithProperties(context, GPU[i], NULL, &status);
        assert(status == CL_SUCCESS);
    }
    /* kernelsource */
    FILE *kernelfp = fopen("matrix-lib.cl", "r");
    assert(kernelfp != NULL);
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
    program = clCreateProgramWithSource(
        context, 1, &constKernelSource, &kernelLength, &status);
    assert(status == CL_SUCCESS);
    /* buildprogram */
    status = clBuildProgram(program, MAXGPU, GPU, NULL, NULL, NULL);
    if (status != CL_SUCCESS) {
        char programBuffer[MAXK];
        size_t log_length;
        status = clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG,
                                       MAXK, programBuffer, &log_length);
        programBuffer[log_length] = '\0';
        printf("%s", programBuffer);
    }
    /* createkernel */
    kernel = clCreateKernel(program, "mul", &status);
    assert(status == CL_SUCCESS);
}
 
int main() {
    int N, S[6];
    build_opencl();
    while(scanf("%d", &N) != EOF) {
        for (int i = 0; i < 6; i++) {
            scanf("%d", &S[i]);
            rand_gen(S[i], N, IN[i]);
        }
        // AB
        multiply(N, IN[0], IN[1], TMP[0], 0);
        // CD
        multiply(N, IN[2], IN[3], TMP[1], MAXGPU-1);
        // AB+CD
        add(N, TMP[0], TMP[1], TMP[2]);
        printf("%u\n", signature(N, TMP[2]));
 
        // ABE
        multiply(N, TMP[0], IN[4], TMP[3], 0);
        // CDF
        multiply(N, TMP[1], IN[5], TMP[4], MAXGPU-1);
        // ABE+CDF
        add(N, TMP[3], TMP[4], TMP[5]);
        printf("%u\n", signature(N, TMP[5]));
    }
    return 0;
}