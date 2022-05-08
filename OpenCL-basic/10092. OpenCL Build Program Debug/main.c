#include <CL/cl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
 
#define MAXGPU 1
#define MAXK 2048
 
/* main */
int main(int argc, char* argv[]) {
    cl_int status;
    cl_platform_id platform_id;
    cl_uint platform_id_got;
    status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
    assert(status == CL_SUCCESS);
 
    // printf("%d platform found\n", platform_id_got);
    cl_device_id GPU;
    cl_uint GPU_id_got;
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &GPU,
                            &GPU_id_got);
    assert(status == CL_SUCCESS);
    // printf("There are %d GPU devices\n", GPU_id_got);
 
    /* getcontext */
    cl_context context =
        clCreateContext(NULL, 1, &GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* kernelsource */
    char filename[50];
    scanf("%s", filename);
    FILE* kernelfp = fopen(filename, "r");
    assert(kernelfp != NULL);
 
    char kernelBuffer[MAXK];
    const char* constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
    // printf("The size of kernel source is %zu\n", kernelLength);
    cl_program program = clCreateProgramWithSource(
        context, 1, &constKernelSource, &kernelLength, &status);
    assert(status == CL_SUCCESS);
 
    /* buildprogram */
    status = clBuildProgram(program, 1, &GPU, NULL, NULL, NULL);
    if (status != CL_SUCCESS) {
        char programBuffer[MAXK];
        size_t log_length;
        status = clGetProgramBuildInfo(program, GPU, CL_PROGRAM_BUILD_LOG, MAXK, programBuffer, &log_length);
        programBuffer[log_length] = '\0';
        printf("%s", programBuffer);
    }
 
    return 0;
}
/* end */