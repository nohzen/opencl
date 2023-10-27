#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "cl_util.h"

#define MAX_PLATFORMS 3
#define MAX_DEVICES 3
#define MAX_SOURCE_SIZE (0x100000)

void show_usage() {
    printf("Usage:\n");
    printf("offiline_compiler [input_kernel.cl] [output_kernel.clbin]\n");
}

int main(int argc, char *argv[]) {
    cl_int ret = CL_SUCCESS;

    if (argc != 3) {
        printf("argc: %d\n", argc);
        show_usage();
        return ret;
    }
    printf("Input kernel file name: %s\n", argv[1]);
    printf("Output kernel file name: %s\n", argv[2]);


    /* Platform */
    cl_platform_id platform_ids[MAX_PLATFORMS] = {0};
    cl_uint num_platforms = 0;
    CL_TRY(clGetPlatformIDs(MAX_PLATFORMS, platform_ids, &num_platforms));
    printf("Platform number = %d\n", num_platforms);

    /* Device */
    cl_device_id device_id[MAX_DEVICES] = {0};
    cl_uint num_devices = 0;
    CL_TRY(clGetDeviceIDs(platform_ids[0], CL_DEVICE_TYPE_DEFAULT, MAX_DEVICES, device_id, &num_devices));
    printf("Device number = %d\n", num_devices);

    /* Context */
    cl_context context = NULL;
    context = clCreateContext(NULL, 1, device_id, NULL, NULL, &ret);
    CL_TRY(ret);

    /* Program */
    FILE *fp = NULL;
    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "error: Failed to load kernel file.\n");
        exit(1);
    }
    char *source_str = (char *)malloc(MAX_SOURCE_SIZE);
    size_t source_size = fread(source_str, sizeof(char), MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_program program = NULL;
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t*)&source_size, &ret);
    CL_TRY(ret);
    CL_TRY(clBuildProgram(program, 1, device_id, NULL, NULL, NULL));

    /* Save compiled program binary */
    size_t program_binary_size;
    CL_TRY(clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &program_binary_size, NULL));
    printf("Program binary size: %ld\n", program_binary_size);

    unsigned char *binary_data = (unsigned char *)malloc(program_binary_size);
    CL_TRY(clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(char*), &binary_data, NULL));

    fp = fopen(argv[2], "w");
    if (!fp) {
        fprintf(stderr, "error: Failed to open output file.\n");
        exit(1);
    }
    fwrite(binary_data, sizeof(unsigned char), program_binary_size, fp);
    fclose(fp);
    free(binary_data);

    /* Release */
    CL_TRY(clReleaseProgram(program));
    CL_TRY(clReleaseContext(context));

    free(source_str);

    return ret;
}