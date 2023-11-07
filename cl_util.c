#include <stdio.h>
#include <stdlib.h>
#include "cl_util.h"


/* Ref:
    https://gist.github.com/allanmac/9328bb2d6a99b86883195f8f78fd1b93
    https://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes */

#define CL_ERR_TO_STR(err) case err: return #err

char const *
clGetErrorString(cl_int const err) {
    switch(err)
    {
      CL_ERR_TO_STR(CL_SUCCESS);
      CL_ERR_TO_STR(CL_DEVICE_NOT_FOUND);
      CL_ERR_TO_STR(CL_DEVICE_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_COMPILER_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
      CL_ERR_TO_STR(CL_OUT_OF_RESOURCES);
      CL_ERR_TO_STR(CL_OUT_OF_HOST_MEMORY);
      CL_ERR_TO_STR(CL_PROFILING_INFO_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_MEM_COPY_OVERLAP);
      CL_ERR_TO_STR(CL_IMAGE_FORMAT_MISMATCH);
      CL_ERR_TO_STR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
      CL_ERR_TO_STR(CL_BUILD_PROGRAM_FAILURE);
      CL_ERR_TO_STR(CL_MAP_FAILURE);
      CL_ERR_TO_STR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
      CL_ERR_TO_STR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
      CL_ERR_TO_STR(CL_COMPILE_PROGRAM_FAILURE);
      CL_ERR_TO_STR(CL_LINKER_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_LINK_PROGRAM_FAILURE);
      CL_ERR_TO_STR(CL_DEVICE_PARTITION_FAILED);
      CL_ERR_TO_STR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_INVALID_VALUE);
      CL_ERR_TO_STR(CL_INVALID_DEVICE_TYPE);
      CL_ERR_TO_STR(CL_INVALID_PLATFORM);
      CL_ERR_TO_STR(CL_INVALID_DEVICE);
      CL_ERR_TO_STR(CL_INVALID_CONTEXT);
      CL_ERR_TO_STR(CL_INVALID_QUEUE_PROPERTIES);
      CL_ERR_TO_STR(CL_INVALID_COMMAND_QUEUE);
      CL_ERR_TO_STR(CL_INVALID_HOST_PTR);
      CL_ERR_TO_STR(CL_INVALID_MEM_OBJECT);
      CL_ERR_TO_STR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
      CL_ERR_TO_STR(CL_INVALID_IMAGE_SIZE);
      CL_ERR_TO_STR(CL_INVALID_SAMPLER);
      CL_ERR_TO_STR(CL_INVALID_BINARY);
      CL_ERR_TO_STR(CL_INVALID_BUILD_OPTIONS);
      CL_ERR_TO_STR(CL_INVALID_PROGRAM);
      CL_ERR_TO_STR(CL_INVALID_PROGRAM_EXECUTABLE);
      CL_ERR_TO_STR(CL_INVALID_KERNEL_NAME);
      CL_ERR_TO_STR(CL_INVALID_KERNEL_DEFINITION);
      CL_ERR_TO_STR(CL_INVALID_KERNEL);
      CL_ERR_TO_STR(CL_INVALID_ARG_INDEX);
      CL_ERR_TO_STR(CL_INVALID_ARG_VALUE);
      CL_ERR_TO_STR(CL_INVALID_ARG_SIZE);
      CL_ERR_TO_STR(CL_INVALID_KERNEL_ARGS);
      CL_ERR_TO_STR(CL_INVALID_WORK_DIMENSION);
      CL_ERR_TO_STR(CL_INVALID_WORK_GROUP_SIZE);
      CL_ERR_TO_STR(CL_INVALID_WORK_ITEM_SIZE);
      CL_ERR_TO_STR(CL_INVALID_GLOBAL_OFFSET);
      CL_ERR_TO_STR(CL_INVALID_EVENT_WAIT_LIST);
      CL_ERR_TO_STR(CL_INVALID_EVENT);
      CL_ERR_TO_STR(CL_INVALID_OPERATION);
      CL_ERR_TO_STR(CL_INVALID_GL_OBJECT);
      CL_ERR_TO_STR(CL_INVALID_BUFFER_SIZE);
      CL_ERR_TO_STR(CL_INVALID_MIP_LEVEL);
      CL_ERR_TO_STR(CL_INVALID_GLOBAL_WORK_SIZE);
      CL_ERR_TO_STR(CL_INVALID_PROPERTY);
      CL_ERR_TO_STR(CL_INVALID_IMAGE_DESCRIPTOR);
      CL_ERR_TO_STR(CL_INVALID_COMPILER_OPTIONS);
      CL_ERR_TO_STR(CL_INVALID_LINKER_OPTIONS);
      CL_ERR_TO_STR(CL_INVALID_DEVICE_PARTITION_COUNT);
      CL_ERR_TO_STR(CL_INVALID_PIPE_SIZE);
      CL_ERR_TO_STR(CL_INVALID_DEVICE_QUEUE);

    default:
        return "UNKNOWN ERROR CODE";
    }
}


cl_int
cl_assert(cl_int const code, char const * const file, int const line, bool const abort) {
    if (code != CL_SUCCESS) {
        char const * const err_str = clGetErrorString(code);

        fprintf(stderr,
            "error: \"%s\", line %d: cl_assert (%d) = %s\n",
            file, line, code, err_str);

        if (abort) {
            // stop profiling and reset device here if necessary
            exit(code);
        }
    }

    return code;
}


#define GET_STR(MACRO_NAME) #MACRO_NAME


void print_platform_info_each(cl_platform_id platform, cl_platform_info param_name, char *param_name_str) {
    size_t param_value_size;
    CL_TRY(clGetPlatformInfo(platform, param_name, 0, NULL, &param_value_size));
    char *param_value = (char *)malloc(sizeof(char) * param_value_size);
    CL_TRY(clGetPlatformInfo(platform, param_name, param_value_size, param_value, NULL));
    printf("    %s: %s\n", param_name_str, param_value);
    free(param_value);
}

void print_platform_info(cl_platform_id platform) {
    print_platform_info_each(platform, CL_PLATFORM_VENDOR, GET_STR(CL_PLATFORM_VENDOR));
    print_platform_info_each(platform, CL_PLATFORM_NAME, GET_STR(CL_PLATFORM_NAME));
    print_platform_info_each(platform, CL_PLATFORM_VERSION, GET_STR(CL_PLATFORM_VERSION));
    print_platform_info_each(platform, CL_PLATFORM_PROFILE, GET_STR(CL_PLATFORM_PROFILE));
    print_platform_info_each(platform, CL_PLATFORM_EXTENSIONS, GET_STR(CL_PLATFORM_EXTENSIONS));
}


void print_device_info_char(cl_device_id device, cl_device_info param_name, char *param_name_str) {
    size_t param_value_size;
    CL_TRY(clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size));
    char *param_value = (char *)malloc(sizeof(char) * param_value_size);
    CL_TRY(clGetDeviceInfo(device, param_name, param_value_size, param_value, NULL));
    printf("    %s: %s\n", param_name_str, param_value);
    free(param_value);
}

cl_bool print_device_info_bool(cl_device_id device, cl_device_info param_name, char *param_name_str) {
    cl_bool param_value;
    CL_TRY(clGetDeviceInfo(device, param_name, sizeof(cl_bool), &param_value, NULL));
    printf("    %s: %d\n", param_name_str, param_value);
    return param_value;
}

cl_uint print_device_info_uint(cl_device_id device, cl_device_info param_name, char *param_name_str) {
    cl_uint param_value;
    CL_TRY(clGetDeviceInfo(device, param_name, sizeof(cl_uint), &param_value, NULL));
    printf("    %s: %d\n", param_name_str, param_value);
    return param_value;
}

cl_ulong print_device_info_cl_ulong(cl_device_id device, cl_device_info param_name, char *param_name_str) {
    cl_ulong param_value;
    CL_TRY(clGetDeviceInfo(device, param_name, sizeof(cl_ulong), &param_value, NULL));
    printf("    %s: %ld\n", param_name_str, param_value);
    return param_value;
}

size_t print_device_info_size_t(cl_device_id device, cl_device_info param_name, char *param_name_str) {
    size_t param_value;
    CL_TRY(clGetDeviceInfo(device, param_name, sizeof(size_t), &param_value, NULL));
    printf("    %s: %ld\n", param_name_str, param_value);
    return param_value;
}

void print_device_info(cl_device_id device) {
    /* General */
    print_device_info_char(device, CL_DEVICE_NAME, GET_STR(CL_DEVICE_NAME));
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
    print_device_info_char(device, CL_DEVICE_VENDOR, GET_STR(CL_DEVICE_VENDOR));
    print_device_info_char(device, CL_DRIVER_VERSION, GET_STR(CL_DRIVER_VERSION));
    print_device_info_char(device, CL_DEVICE_PROFILE, GET_STR(CL_DEVICE_PROFILE));
    print_device_info_char(device, CL_DEVICE_VERSION, GET_STR(CL_DEVICE_VERSION));
    print_device_info_char(device, CL_DEVICE_OPENCL_C_VERSION, GET_STR(CL_DEVICE_OPENCL_C_VERSION));
    // print_device_info_char(device, CL_DEVICE_EXTENSIONS, GET_STR(CL_DEVICE_EXTENSIONS));

    /* Work item */
    print_device_info_uint(device, CL_DEVICE_MAX_COMPUTE_UNITS, GET_STR(CL_DEVICE_MAX_COMPUTE_UNITS));
    {
        cl_uint work_dim = print_device_info_uint(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, GET_STR(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS));
        size_t *work_item_sizes = (size_t *)malloc(work_dim * sizeof(size_t));
        CL_TRY(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, work_dim * sizeof(size_t), work_item_sizes, NULL));
        printf("    %s:\n", GET_STR(CL_DEVICE_MAX_WORK_ITEM_SIZES));
        for (int i = 0; i < work_dim; i++) {
            printf("      %d: %ld in work group\n", i, work_item_sizes[i]);
        }
        free(work_item_sizes);
    }
    print_device_info_size_t(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, GET_STR(CL_DEVICE_MAX_WORK_GROUP_SIZE));
    // print_device_info_size_t(device, CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, GET_STR(CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE));

    /* Vector (SIMD) */
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR));
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT));
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT));
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG));
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT));
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE));
    print_device_info_uint(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, GET_STR(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE));
    print_device_info_uint(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, GET_STR(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF));

    /* Spec */
    print_device_info_uint(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, GET_STR(CL_DEVICE_MAX_CLOCK_FREQUENCY)); // MHz
    print_device_info_uint(device, CL_DEVICE_ADDRESS_BITS, GET_STR(CL_DEVICE_ADDRESS_BITS));

    {
        cl_device_local_mem_type local_mem_type;
        CL_TRY(clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &local_mem_type, NULL));
        if (local_mem_type == CL_LOCAL) {
            printf("    %s: %s\n", "CL_DEVICE_LOCAL_MEM_TYPE", "CL_LOCAL");
            // dedicated local memory storage such as SRAM
        }
        else if (local_mem_type == CL_GLOBAL) {
            printf("    %s: %s\n", "CL_DEVICE_LOCAL_MEM_TYPE", "CL_GLOBAL");
        }
    }
    print_device_info_cl_ulong(device, CL_DEVICE_LOCAL_MEM_SIZE, GET_STR(CL_DEVICE_LOCAL_MEM_SIZE)); // bytes

    /* Image */
    print_device_info_bool(device, CL_DEVICE_IMAGE_SUPPORT, GET_STR(CL_DEVICE_IMAGE_SUPPORT));
    print_device_info_uint(device, CL_DEVICE_MAX_READ_IMAGE_ARGS, GET_STR(CL_DEVICE_MAX_READ_IMAGE_ARGS));
    print_device_info_uint(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, GET_STR(CL_DEVICE_MAX_WRITE_IMAGE_ARGS));
    print_device_info_size_t(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, GET_STR(CL_DEVICE_IMAGE2D_MAX_WIDTH));
    print_device_info_size_t(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, GET_STR(CL_DEVICE_IMAGE2D_MAX_HEIGHT));
    print_device_info_size_t(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, GET_STR(CL_DEVICE_IMAGE3D_MAX_WIDTH));
    print_device_info_size_t(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, GET_STR(CL_DEVICE_IMAGE3D_MAX_HEIGHT));
    print_device_info_size_t(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, GET_STR(CL_DEVICE_IMAGE3D_MAX_DEPTH));

}


void print_program_info_each(cl_program program, cl_program_info param_name, char *param_name_str) {
    size_t param_value_size;
    CL_TRY(clGetProgramInfo(program, param_name, 0, NULL, &param_value_size));
    char *param_value = (char *)malloc(sizeof(char) * param_value_size);
    CL_TRY(clGetProgramInfo(program, param_name, param_value_size, param_value, NULL));
    printf("    %s: %s\n", param_name_str, param_value);
    free(param_value);
}

void print_program_info(cl_program program) {
    print_program_info_each(program, CL_PROGRAM_SOURCE, GET_STR(CL_PROGRAM_SOURCE));
}


void print_kernel_info_each_size_t(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, char *param_name_str) {
    size_t param_value;
    CL_TRY(clGetKernelWorkGroupInfo(kernel, device, param_name, sizeof(size_t), &param_value, NULL));
    printf("    %s: %ld\n", param_name_str, param_value);
}

void print_kernel_info_each_ulong(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, char *param_name_str) {
    cl_ulong param_value;
    CL_TRY(clGetKernelWorkGroupInfo(kernel, device, param_name, sizeof(cl_ulong), &param_value, NULL));
    printf("    %s: %lu\n", param_name_str, param_value);
}

void print_kernel_info(cl_kernel kernel, cl_device_id device) {
    printf("Kernel info\n");
#if 0
    {
        printf("    %s:\n", GET_STR(CL_KERNEL_GLOBAL_WORK_SIZE));
        size_t param_value[3];
        CL_TRY(clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_GLOBAL_WORK_SIZE, 3*sizeof(size_t), param_value, NULL));
        printf("      (%ld, %ld, %ld) \n", param_value[0], param_value[1], param_value[2]);
    }
#endif
    print_kernel_info_each_size_t(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, GET_STR(CL_KERNEL_WORK_GROUP_SIZE));
    print_kernel_info_each_size_t(kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, GET_STR(CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE)); // N * local_work_size
    print_kernel_info_each_ulong(kernel, device, CL_KERNEL_LOCAL_MEM_SIZE, GET_STR(CL_KERNEL_LOCAL_MEM_SIZE));
    print_kernel_info_each_ulong(kernel, device, CL_KERNEL_PRIVATE_MEM_SIZE, GET_STR(CL_KERNEL_PRIVATE_MEM_SIZE));
}

