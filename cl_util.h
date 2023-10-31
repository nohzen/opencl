#pragma once

#include <CL/cl.h>
#include <stdbool.h>


cl_int cl_assert(cl_int const code, char const * const file, int const line, bool const abort);

#define CL_TRY(...) cl_assert(__VA_ARGS__, __FILE__, __LINE__, true);
#define cl_ok(err)  cl_assert(err, __FILE__, __LINE__, true);


void print_platform_info(cl_platform_id platform);

void print_device_info(cl_device_id device);

void print_program_info(cl_program program);

void print_kernel_info(cl_kernel kernel, cl_device_id device);


