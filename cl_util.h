#pragma once

#include <CL/cl.h>
#include <stdbool.h>

char const * clGetErrorString(cl_int const err);

cl_int cl_assert(cl_int const code, char const * const file, int const line, bool const abort);

#define CL_TRY(...) cl_assert(__VA_ARGS__, __FILE__, __LINE__, true);
#define cl_ok(err)  cl_assert(err, __FILE__, __LINE__, true);
