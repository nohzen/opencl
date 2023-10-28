#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "cl_util.h"


#define MAX_PLATFORMS 3
#define MAX_DEVICES 3
#define MATRIX_SIZE 4
#define MAX_SOURCE_SIZE (0x100000)

int main() {
    cl_int ret = CL_SUCCESS;

    /* Platform */
    cl_platform_id platform_ids[MAX_PLATFORMS] = {0};
    cl_uint num_platforms = 0;
    CL_TRY(clGetPlatformIDs(MAX_PLATFORMS, platform_ids, &num_platforms));
    printf("Platform number = %d\n", num_platforms);
    for (int i = 0; i < num_platforms; i++) {
        printf("  Platform info: %d\n", i);
        print_platform_info(platform_ids[i]);
    }

    /* Device */
    cl_device_id device_id[MAX_DEVICES] = {0};
    cl_uint num_devices = 0;
    // CL_DEVICE_TYPE_DEFAULT, CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_ALL
    CL_TRY(clGetDeviceIDs(platform_ids[0], CL_DEVICE_TYPE_DEFAULT, MAX_DEVICES, device_id, &num_devices));
    printf("Device number = %d\n", num_devices);
    for (int i = 0; i < num_devices; i++) {
        printf("  Device info: %d\n", i);
        print_device_info(device_id[i]);
    }

    /* Context */
    cl_context context = NULL;
    context = clCreateContext(NULL, 1, device_id, NULL, NULL, &ret);
    CL_TRY(ret);

    /* Command queue */
    cl_command_queue command_queue = NULL;
    // command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret); // deprecated
    const cl_queue_properties *properties = NULL;
    // const cl_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, 0};
    command_queue = clCreateCommandQueueWithProperties(context, device_id[0], properties, &ret);
    CL_TRY(ret);

    /* Memory Object */
    cl_mem memobjA = NULL;
    cl_mem memobjB = NULL;
    cl_mem memobjC = NULL;
    memobjA = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE*MATRIX_SIZE*sizeof(float), NULL, &ret);
    memobjB = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE*MATRIX_SIZE*sizeof(float), NULL, &ret);
    memobjC = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE*MATRIX_SIZE*sizeof(float), NULL, &ret);
    CL_TRY(ret);

    float memA[MATRIX_SIZE*MATRIX_SIZE];
    float memB[MATRIX_SIZE*MATRIX_SIZE];
    float memC[MATRIX_SIZE*MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            memA[i*MATRIX_SIZE + j] = i*MATRIX_SIZE + j + 1;
            memB[i*MATRIX_SIZE + j] = j*MATRIX_SIZE + i + 1;
        }
    }
    CL_TRY(clEnqueueWriteBuffer(command_queue, memobjA, CL_TRUE, 0, MATRIX_SIZE*MATRIX_SIZE*sizeof(float), memA, 0, NULL, NULL));
    CL_TRY(clEnqueueWriteBuffer(command_queue, memobjB, CL_TRUE, 0, MATRIX_SIZE*MATRIX_SIZE*sizeof(float), memB, 0, NULL, NULL));

    /* Program */
    int use_binary = 0;
    FILE *fp = NULL;
    if (use_binary) {
        char filename[] = "kernel.clbin";
        fp = fopen(filename, "r");
    }
    else {
        char filename[] = "kernel.cl";
        fp = fopen(filename, "r");
    }
    if (!fp) {
        fprintf(stderr, "error: Failed to load kernel file.\n");
        exit(1);
    }
    char *source_str = (char *)malloc(MAX_SOURCE_SIZE);
    size_t source_size = fread(source_str, sizeof(char), MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_program program = NULL;
    if (use_binary) {
        cl_int binary_status;
        program = clCreateProgramWithBinary(context, 1, device_id, (const size_t *)&source_size, (const unsigned char **)&source_str, &binary_status, &ret);
        CL_TRY(ret);
        CL_TRY(clBuildProgram(program, 1, device_id, NULL, NULL, NULL)); // Need on Intel GPU
    }
    else {
        program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
        CL_TRY(ret);
        CL_TRY(clBuildProgram(program, 1, device_id, NULL, NULL, NULL));
    }

    /* Kernel */
    cl_kernel kernel = NULL;
    kernel = clCreateKernel(program, "dataParallel", &ret);
    CL_TRY(ret);
    CL_TRY(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjA));
    CL_TRY(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobjB));
    CL_TRY(clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobjC));

    // CL_TRY(clEnqueueTask(command_queue, kernel, 0, NULL, NULL)); // deprecated
    size_t global_work_size[3] = {MATRIX_SIZE*MATRIX_SIZE, 0, 0};
    size_t local_work_size[3] = {1, 0, 0};
    CL_TRY(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL));

    /* Read */
    CL_TRY(clEnqueueReadBuffer(command_queue, memobjC, CL_TRUE, 0, MATRIX_SIZE*MATRIX_SIZE*sizeof(float), memC, 0, NULL, NULL));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("memC[%d, %d]: %7.2f    ", i, j, memC[i*MATRIX_SIZE + j]);
        }
        printf("\n");
    }

    /* Release */
    CL_TRY(clFlush(command_queue));
    CL_TRY(clFinish(command_queue));

    CL_TRY(clReleaseKernel(kernel));
    CL_TRY(clReleaseProgram(program));
    CL_TRY(clReleaseMemObject(memobjA));
    CL_TRY(clReleaseMemObject(memobjB));
    CL_TRY(clReleaseMemObject(memobjC));
    CL_TRY(clReleaseCommandQueue(command_queue));
    CL_TRY(clReleaseContext(context));

    free(source_str);

    return ret;
}