#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__kernel void hello(__global char* string) {
   string[0] = 'H';
   string[1] = 'e';
   string[2] = 'l';
   string[3] = 'l';
   string[4] = 'o';
   string[5] = ',';
   string[6] = ' ';
   string[7] = 'W';
   string[8] = 'o';
   string[9] = 'r';
   string[10] = 'l';
   string[11] = 'd';
   string[12] = '!';
   string[13] = '\0';
}

__kernel void vecAdd(__global float* a) {
   int gid = get_global_id(0);
   a[gid] += a[gid];
}

__kernel void dataParallel(__global float* A, __global float* B, __global float* C) {
   int base = 4 * get_global_id(0);

   C[base+0] = A[base+0] + B[base+0];
   C[base+1] = A[base+1] - B[base+1];
   C[base+2] = A[base+2] * B[base+2];
   C[base+3] = A[base+3] / B[base+3];
}

__kernel void local_test(__local int *p, int local_size) {
   for (int i=0; i<local_size; i++) {
      p[i] = i;
   }
}
