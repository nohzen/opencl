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


const sampler_t s_nearest = CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;
const sampler_t s_linear = CLK_FILTER_LINEAR | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;
const sampler_t s_repeat = CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_REPEAT;

__kernel void
image_test(__read_only image2d_t im,
           __global float4 *out) {
   /* nearest */
   out[0] = read_imagef(im, s_nearest, (float2)(0.5f,0.5f));
   out[1] = read_imagef(im, s_nearest, (float2)(0.8f,0.5f));
   out[2] = read_imagef(im, s_nearest, (float2)(1.3f,0.5f));

   /* linear */
   out[3] = read_imagef(im, s_linear, (float2)(0.5f,0.5f));
   out[4] = read_imagef(im, s_linear, (float2)(0.8f,0.5f));
   out[5] = read_imagef(im, s_linear, (float2)(1.3f,0.5f));

   /* repeat */
   out[6] = read_imagef(im, s_repeat, (float2)(4.5f,0.5f));
   out[7] = read_imagef(im, s_repeat, (float2)(5.0f,0.5f));
   out[8] = read_imagef(im, s_repeat, (float2)(6.5f,0.5f));
}
