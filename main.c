#include <stdio.h>
#include "cl_util.h"
#include <CL/cl.h>



void print_platform_info_each(cl_platform_id platform, cl_platform_info param_name, char *param_name_str) {
    size_t param_value_size;
    CL_TRY(clGetPlatformInfo(platform, param_name, 0, NULL, &param_value_size));
    char *param_value = (char *)malloc(sizeof(char) * param_value_size);
    CL_TRY(clGetPlatformInfo(platform, param_name, param_value_size, param_value, NULL));
    printf("    %s: %s\n", param_name_str, param_value);
    free(param_value);
}

#define GET_STR(MACRO_NAME) #MACRO_NAME

void print_platform_info(cl_platform_id platform) {
    print_platform_info_each(platform, CL_PLATFORM_VENDOR, GET_STR(CL_PLATFORM_VENDOR));
    print_platform_info_each(platform, CL_PLATFORM_NAME, GET_STR(CL_PLATFORM_NAME));
    print_platform_info_each(platform, CL_PLATFORM_VERSION, GET_STR(CL_PLATFORM_VERSION));
    print_platform_info_each(platform, CL_PLATFORM_PROFILE, GET_STR(CL_PLATFORM_PROFILE));
    // print_platform_info_each(platform, CL_PLATFORM_EXTENSIONS, GET_STR(CL_PLATFORM_EXTENSIONS));
}


void print_device_info_each(cl_device_id device, cl_device_info param_name, char *param_name_str) {
    size_t param_value_size;
    CL_TRY(clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size));
    char *param_value = (char *)malloc(sizeof(char) * param_value_size);
    CL_TRY(clGetDeviceInfo(device, param_name, param_value_size, param_value, NULL));
    printf("    %s: %s\n", param_name_str, param_value);
    free(param_value);
}

void print_device_info(cl_device_id device) {
    print_device_info_each(device, CL_DEVICE_NAME, GET_STR(CL_DEVICE_NAME));
    {
        cl_device_type device_type;
        CL_TRY(clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL));
        if (device_type == CL_DEVICE_TYPE_DEFAULT) {
            printf("    %s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_DEFAULT");
        }
        else if (device_type == CL_DEVICE_TYPE_CPU) {
            printf("    %s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_CPU");
        }
        else if (device_type == CL_DEVICE_TYPE_GPU) {
            printf("    %s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_GPU");
        }
        else if (device_type == CL_DEVICE_TYPE_ACCELERATOR) {
            printf("    %s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_ACCELERATOR");
        }
    }
    print_device_info_each(device, CL_DEVICE_VENDOR, GET_STR(CL_DEVICE_VENDOR));
    print_device_info_each(device, CL_DRIVER_VERSION, GET_STR(CL_DRIVER_VERSION));
    print_device_info_each(device, CL_DEVICE_PROFILE, GET_STR(CL_DEVICE_PROFILE));
    print_device_info_each(device, CL_DEVICE_VERSION, GET_STR(CL_DEVICE_VERSION));
    print_device_info_each(device, CL_DEVICE_OPENCL_C_VERSION, GET_STR(CL_DEVICE_OPENCL_C_VERSION));
    // print_device_info_each(device, CL_DEVICE_EXTENSIONS, GET_STR(CL_DEVICE_EXTENSIONS));
}



#define MAX_PLATFORMS 3
#define MAX_DEVICES 3
#define MEM_SIZE 128
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
    command_queue = clCreateCommandQueueWithProperties(context, device_id[0], NULL, &ret);
    CL_TRY(ret);

    /* Memory Object */
    cl_mem memobj = NULL;
    memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE*sizeof(char) , NULL, &ret);
    CL_TRY(ret);

    /* Kernel, Program */
    FILE *fp = NULL;
    char filename[] = "kernel.cl";
    fp = fopen(filename, "r");
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

    cl_kernel kernel = NULL;
    kernel = clCreateKernel(program, "hello", &ret);
    CL_TRY(ret);
    CL_TRY(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj));

    // CL_TRY(clEnqueueTask(command_queue, kernel, 0, NULL, NULL)); // deprecated
    size_t global_work_size[1] = {1};
    size_t local_work_size[1] = {1};
    CL_TRY(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL));












    return ret;
}