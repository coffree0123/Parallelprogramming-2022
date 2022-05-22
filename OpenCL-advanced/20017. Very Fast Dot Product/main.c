#include <stdio.h>
#include <assert.h>
#include <CL/cl.h>
#include <inttypes.h>
 
#define MAXGPU 1
#define MAXK 4096
#define MAXN 67108864
#define MAXLOG 8192
#define GROUP_SIZE 1024
 
int main(int argc, char *argv[])
{
    cl_int status;
    cl_platform_id platform_id;
    cl_uint platform_id_got;
    status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
    assert(status == CL_SUCCESS);
    cl_device_id GPU[MAXGPU];
    cl_uint GPU_id_got;
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU,
                            MAXGPU, GPU, &GPU_id_got);
    assert(status == CL_SUCCESS);
 
    /* getcontext */
    cl_context context = 
        clCreateContext(NULL, 1, GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* commandqueue */
    cl_command_queue commandQueue =
        clCreateCommandQueueWithProperties(context, GPU[0], NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* kernelsource */
    FILE *kernelfp = fopen("vecdot.cl", "r");
    assert(kernelfp != NULL);
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = 
        fread(kernelBuffer, 1, MAXK, kernelfp);
    cl_program program = 
        clCreateProgramWithSource(context, 1, &constKernelSource, 
                                  &kernelLength, &status);
    assert(status == CL_SUCCESS);
 
    /* buildprogram */
    status = 
        clBuildProgram(program, 1, GPU, NULL, NULL, NULL);
    // assert(status == CL_SUCCESS);
    if (status != CL_SUCCESS) {
        char program_log[MAXLOG];
        status = 
            clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG,
                                  sizeof(program_log), program_log, NULL);
        printf("%s", program_log);
        exit(1);
    }
 
    /* createkernel */
    cl_kernel kernel = clCreateKernel(program, "vecdot", &status);
    assert(status == CL_SUCCESS);
 
    /* vector */
    cl_uint* C = (cl_uint*)malloc(MAXN * sizeof(cl_uint));
    assert(C != NULL);
 
    /* createbuffer */
    cl_mem bufferC = 
        clCreateBuffer(context,
                       CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                       MAXN * sizeof(cl_uint), C, &status);
    assert(status == CL_SUCCESS);
 
    /* declare */
    int N;
    uint32_t key1, key2;
    while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
        /* setarg */
        status = clSetKernelArg(kernel, 0, sizeof(uint32_t), 
                                (void*)&key1);
        assert(status == CL_SUCCESS);
        status = clSetKernelArg(kernel, 1, sizeof(uint32_t), 
                                (void*)&key2);
        assert(status == CL_SUCCESS);
        status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
                                (void*)&bufferC);
        assert(status == CL_SUCCESS);
        status = clSetKernelArg(kernel, 3, sizeof(int), 
                                (void*)&N);
        assert(status == CL_SUCCESS);
 
        /* setshape */
        size_t total_group = N / GROUP_SIZE + 1;
        if (total_group % GROUP_SIZE != 0)
            total_group = (total_group/GROUP_SIZE + 1) * GROUP_SIZE;
        size_t globalThreads[] = {(size_t)total_group};
        size_t localThreads[] = {GROUP_SIZE};
        status = 
            clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, 
                                   globalThreads, localThreads, 
                                   0, NULL, NULL);
        assert(status == CL_SUCCESS);
 
        /* getcvector */
        clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
                            0, total_group * sizeof(cl_uint), C, 
                            0, NULL, NULL);
 
        /* sum */
        uint32_t sum = 0;
        for (int i = 0; i < total_group; i++)
            sum += C[i];
        printf("%" PRIu32 "\n", sum);
    }
 
    /* free */
    free(C);
    clReleaseContext(context);
    clReleaseCommandQueue(commandQueue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(bufferC);
 
    return 0;
}