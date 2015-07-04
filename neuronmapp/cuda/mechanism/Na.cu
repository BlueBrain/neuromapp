#include "Na_kernels.h"
#include <sys/time.h>

__global__ void Na_state_kernel(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                double* __restrict pion_data0, int * __restrict ppvar0, double* __restrict pVEC_V, size_t size)
{
        unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

        if (idx < size) {
            NA_STATE(p_0, p_1, p_2, p_3, pion_data0, ppvar0, pVEC_V, idx);
        }

        __syncthreads();
}



void Na_state_wrapper(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                double* __restrict pion_data0, int * __restrict ppvar0, double* __restrict pVEC_V, size_t size)
{
        dim3 block(512);
        dim3 grid(size/block.x + 1);

        struct timeval start, stop;
        gettimeofday(&start, 0);

        Na_state_kernel<<<grid, block>>>(p_0, p_1, p_2, p_3, pion_data0, ppvar0, pVEC_V, size);

        cudaDeviceSynchronize();

        gettimeofday(&stop, 0);

        printf("GPU State %f s\n", 1e-6 * (stop.tv_usec - start.tv_usec) + (float) (stop.tv_sec - start.tv_sec));
}


__global__ void Na_current_kernel(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                int * __restrict ppvar0, int * __restrict ppvar1, int * __restrict ppvar2, double* __restrict pion_data0,
                double* __restrict pion_data1, double* __restrict pion_data2, double* __restrict pVEC_V,
                double* __restrict pVEC_RHS, int * __restrict pni, size_t size)
{
        unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

        if (idx < size) {
            NA_CURRENT(p_0, p_1, p_2, p_3, ppvar0, ppvar1, ppvar2, pion_data0, pion_data1, pion_data2, pVEC_V, pVEC_RHS, pni, idx);
        }

        //__syncthreads();

        // Implementation for 1 GPU thread
        //for (int i = 0; i < size; i++) {
        //    NA_CURRENT(p_0, p_1, p_2, p_3, ppvar0, ppvar1, ppvar2, pion_data0, pion_data1, pion_data2, pVEC_V, pVEC_RHS, pni, i);
        //}

        //__syncthreads();

        // Implementation for 1 thread block
        //int stride = size / blockDim.x;
        //int first = idx * stride;
        //int last = (idx+1) * stride;
        //for (int i = first; i < last; i++) {
        //    NA_CURRENT(p_0, p_1, p_2, p_3, ppvar0, ppvar1, ppvar2, pion_data0, pion_data1, pion_data2, pVEC_V, pVEC_RHS, pni, i);
        //}

        //__syncthreads();

        //if (stride * blockDim.x < size) {
        //    idx = blockDim.x * stride + idx;

        //    if (idx < size) {
        //        NA_CURRENT(p_0, p_1, p_2, p_3, ppvar0, ppvar1, ppvar2, pion_data0, pion_data1, pion_data2, pVEC_V, pVEC_RHS, pni, idx);
        //    }

        //}

        //__syncthreads();
}

void Na_current_wrapper(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                int * __restrict ppvar0, int * __restrict ppvar1, int * __restrict ppvar2, double* __restrict pion_data0,
                double* __restrict pion_data1, double* __restrict pion_data2, double* __restrict pVEC_V,
                double* __restrict pVEC_RHS, int * __restrict pni, size_t size)
{
        dim3 block(512);
        dim3 grid(size/block.x + 1);
        //dim3 block(1);
        //dim3 grid(1);

        struct timeval start, stop;
        gettimeofday(&start, 0);

        Na_current_kernel<<<grid, block>>>(p_0, p_1, p_2, p_3, ppvar0, ppvar1, ppvar2, pion_data0, pion_data1, pion_data2, pVEC_V, pVEC_RHS, pni, size);

        cudaDeviceSynchronize();

        gettimeofday(&stop, 0);

        printf("GPU Current %f s\n", 1e-6 * (stop.tv_usec - start.tv_usec) + (float) (stop.tv_sec - start.tv_sec));
}
