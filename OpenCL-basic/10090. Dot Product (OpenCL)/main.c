#include <CL/cl.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include "utils.h"
#define MAXGPU 8
#define MAXK 1024
#define GroupSize 1024
#define MAXN 16777216
uint32_t C[MAXN / GroupSize];

int main() {
    cl_int status;
    cl_platform_id platform_id[5];
    cl_uint platform_id_got;
    status = clGetPlatformIDs(5, platform_id, &platform_id_got);
    // printf("platform_id_got %d\n", platform_id_got);
    cl_device_id GPU[10];
    cl_uint GPU_id_got;

    status = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, MAXGPU, GPU,
                            &GPU_id_got);
    // printf("GPU_id_got %d\n", GPU_id_got);
    cl_context context = clCreateContext(NULL, 1, GPU, NULL, NULL, &status);
    cl_command_queue commandQueue =
        clCreateCommandQueueWithProperties(context, GPU[0], NULL, &status);

    FILE *kernelfp = fopen("vecdot.cl", "r");
    assert(kernelfp != NULL);
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);

    cl_program program = clCreateProgramWithSource(
        context, 1, &constKernelSource, &kernelLength, &status);
    status = clBuildProgram(program, 1, GPU, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "vecdot", &status);

    cl_mem buffer =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                        sizeof(C), C, &status);

    int N;
    uint32_t key1, key2;
    while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
        uint32_t sum = 0;
        while (N % GroupSize != 0) {
            sum -= encrypt(N, key1) * encrypt(N, key2);
            N++;
        }


        size_t globalSize[] = {N};
        size_t localSize[] = {GroupSize};

        clSetKernelArg(kernel, 0, sizeof(cl_uint), (void *)&key1);
        clSetKernelArg(kernel, 1, sizeof(cl_uint), (void *)&key2);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&buffer);

        clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalSize,
                               localSize, 0, NULL, NULL);
        clEnqueueReadBuffer(commandQueue, buffer, CL_TRUE, 0, sizeof(C), C, 0,
                            NULL, NULL);

        for (int i = 0; i < N / GroupSize; i++) sum += C[i];
        printf("%" PRIu32 "\n", sum);
    }

    return 0;
}