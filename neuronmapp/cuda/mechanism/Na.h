#pragma once

#include <cuda_runtime.h>


#include "Na_kernels.h"

#define CHECK_CPU 0

#if CHECK_CPU
#include <string.h>
#endif

#include <unistd.h>

void Na_state_wrapper(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                double* __restrict pion_data0, int * __restrict ppvar0, double* __restrict pVEC_V, size_t size);

void Na_current_wrapper(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                int * __restrict ppvar0, int * __restrict ppvar1, int * __restrict ppvar2, double* __restrict pion_data0,
                double* __restrict pion_data1, double* __restrict pion_data2, double* __restrict pVEC_V,
                double* __restrict pVEC_RHS, int * __restrict pni, size_t size);


class Na : public mechanism{
public:
    static const int width = 10;
    static std::string name(){return "Na";};

    Na(size_t s, size_t l): mechanism(s,l){}

    void state(){
        double* __restrict p_0 =  (double *)( &c.front()+0*chunk());
        double* __restrict p_1 =  (double *)( &c.front()+1*chunk());
        double* __restrict p_2 =  (double *)( &c.front()+2*chunk());
        double* __restrict p_3 =  (double *)( &c.front()+3*chunk());

        double* __restrict pion_data0 =  (double *)( &ion_data.front()+0*chunk());  

        int * __restrict ppvar0 =  (int *)( &ppvar.front()+0*chunk());  

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());  

        size_t size =  number_instance();


#if CHECK_CPU
        /************* CPU VERIFICATION ****************/
        double* __restrict p_0cpu = (double *) malloc(size * sizeof(double));
        double* __restrict p_1cpu = 0; // Not used in kernel
        double* __restrict p_2cpu = (double *) malloc(size * sizeof(double));
        double* __restrict p_3cpu = (double *) malloc(size * sizeof(double));
        memcpy(p_0cpu, p_0, size * sizeof(double));
        memcpy(p_2cpu, p_2, size * sizeof(double));
        memcpy(p_3cpu, p_3, size * sizeof(double));
        /**********************************************/
#endif


        // CUDA device pointers
        double* __restrict p_0d;
        double* __restrict p_1d = 0; // Not used in kernel
        double* __restrict p_2d;
        double* __restrict p_3d;
        double* __restrict pion_data0d;
        int * __restrict ppvar0d;
        double* __restrict pVEC_Vd;

        cudaMalloc((void **)&p_0d, size * sizeof(double));
        cudaMalloc((void **)&p_2d, size * sizeof(double));
        cudaMalloc((void **)&p_3d, size * sizeof(double));
        cudaMalloc((void **)&pion_data0d, size * 3 * sizeof(double));
        cudaMalloc((void **)&ppvar0d, size * sizeof(int));
        cudaMalloc((void **)&pVEC_Vd, size * sizeof(double));

        cudaMemcpy(p_0d, p_0, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(p_2d, p_2, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(p_3d, p_3, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(pion_data0d, pion_data0, size * 3 * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(ppvar0d, ppvar0, size * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(pVEC_Vd, pVEC_V, size * sizeof(double), cudaMemcpyHostToDevice);

        Na_state_wrapper(p_0d, p_1d, p_2d, p_3d, pion_data0d, ppvar0d, pVEC_Vd, size);

        cudaMemcpy(p_0, p_0d, size * sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(p_2, p_2d, size * sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(p_3, p_3d, size * sizeof(double), cudaMemcpyDeviceToHost);

        cudaFree(p_0d);
        cudaFree(p_2d);
        cudaFree(p_3d);
        cudaFree(pion_data0d);
        cudaFree(ppvar0d);
        cudaFree(pVEC_Vd);

        cudaError_t cuerr = cudaGetLastError();
        if (cuerr != cudaSuccess)
            std::cout << "WARNING: CUDA Error found " << cudaGetErrorString(cuerr) << std::endl;

#if CHECK_CPU
        /************* CPU VERIFICATION ****************/
        double dt = 0.1;
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _lhAlpha , _lhBeta , _lhInf , _lhTau , _llv , _lqt, _v ;

        _lqt = 2.952882641412121 ;

        #pragma ibm independent_loop
        #pragma GCC ivdep
        for (int i = 0; i < size; i++) {
            NA_STATE(p_0cpu, p_1cpu, p_2cpu, p_3cpu, pion_data0, ppvar0, pVEC_V, i);
        }

        int err0 = 0, err2 = 0, err3 = 0;
        for (int i = 0; i < size; i++) {
            if (fabs(p_0[i] - p_0cpu[i]) > 1e-16) {
                err0++;
                std::cout << "Expected: " << p_0cpu[i] << " but found: " << p_0[i] << std::endl;
                printf("%.16f    %.16f\n", p_0cpu[i], p_0[i]);
            }
            if (fabs(p_2[i] - p_2cpu[i]) > 1e-16) err2++;
            if (fabs(p_3[i] - p_3cpu[i]) > 1e-16) err3++;
        }
        if (err0 != 0)
            std::cout << err0 << " errors found in p_0!" << std::endl;
        if (err2 != 0)
            std::cout << err2 << " errors found in p_2!" << std::endl;
        if (err3 != 0)
            std::cout << err3 << " errors found in p_3!" << std::endl;

        if (err0 == 0 && err2 == 0 && err3 == 0)
            std::cout << "Na state() GPU computation verified" << std::endl;
        /*****************************/
#endif
    }

    void current(){
        double* __restrict p_0 =  (double *)( &c.front()+0*chunk());
        double* __restrict p_1 =  (double *)( &c.front()+1*chunk());
        double* __restrict p_2 =  (double *)( &c.front()+2*chunk());
        double* __restrict p_3 =  (double *)( &c.front()+3*chunk());
        
        int * __restrict ppvar0 =  (int *)( &ppvar.front()+0*chunk());  
        int * __restrict ppvar1 =  (int *)( &ppvar.front()+1*chunk());  
        int * __restrict ppvar2 =  (int *)( &ppvar.front()+2*chunk());  

        double* __restrict pion_data0 =  (double *)( &ion_data.front()+0*chunk());  
        double* __restrict pion_data1 =  (double *)( &ion_data.front()+1*chunk());  
        double* __restrict pion_data2 =  (double *)( &ion_data.front()+2*chunk());  

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());  
        double* __restrict pVEC_RHS =  (double *)( &VEC_RHS.front());  

        int* __restrict pni = (int *)(&ni.front());

        double rhs, dina;
        size_t size =  number_instance();

#if CHECK_CPU
        /************* CPU VERIFICATION ****************/
        double* __restrict p_3cpu = (double *) malloc(size * sizeof(double));
        double* __restrict pion_data0cpu = (double *) malloc(ion_data.size() * sizeof(double));
        double* __restrict pion_data1cpu = (double *)( &pion_data0cpu[0]+1*chunk());
        double* __restrict pion_data2cpu = (double *)( &pion_data0cpu[0]+2*chunk());
        double* __restrict pVEC_Vcpu = (double *) malloc(size * sizeof(double));
        double* __restrict pVEC_RHScpu = (double *) malloc(size * sizeof(double));
        memcpy(p_3cpu, p_3, size * sizeof(double));
        memcpy(pion_data0cpu, pion_data0, ion_data.size() * sizeof(double));
        memcpy(pVEC_Vcpu, pVEC_V, size * sizeof(double));
        memcpy(pVEC_RHScpu, pVEC_RHS, size * sizeof(double));

        // Double-check CPU
        double* __restrict p_3cpu2 = (double *) malloc(size * sizeof(double));
        double* __restrict pion_data0cpu2 = (double *) malloc(ion_data.size() * sizeof(double));
        double* __restrict pion_data1cpu2 = (double *)( &pion_data0cpu2[0]+1*chunk());
        double* __restrict pion_data2cpu2 = (double *)( &pion_data0cpu2[0]+2*chunk());
        double* __restrict pVEC_Vcpu2 = (double *) malloc(size * sizeof(double));
        double* __restrict pVEC_RHScpu2 = (double *) malloc(size * sizeof(double));
        memcpy(p_3cpu2, p_3, size * sizeof(double));
        memcpy(pion_data0cpu2, pion_data0, ion_data.size() * sizeof(double));
        memcpy(pVEC_Vcpu2, pVEC_V, size * sizeof(double));
        memcpy(pVEC_RHScpu2, pVEC_RHS, size * sizeof(double));
        /**********************************************/
#endif

        // CUDA device pointers
        double* __restrict p_0d;
        double* __restrict p_1d;
        double* __restrict p_2d;
        double* __restrict p_3d;
        int * __restrict ppvard;
        int * __restrict ppvar0d;
        int * __restrict ppvar1d;
        int * __restrict ppvar2d;
        double* __restrict pion_datad;
        double* __restrict pion_data0d;
        double* __restrict pion_data1d;
        double* __restrict pion_data2d;
        double* __restrict pVEC_Vd;
        double* __restrict pVEC_RHSd;
        int * __restrict pnid;

        // CUDA allocation on GPU
        cudaMalloc((void **)&p_0d, size * sizeof(double));
        cudaMalloc((void **)&p_1d, size * sizeof(double));
        cudaMalloc((void **)&p_2d, size * sizeof(double));
        cudaMalloc((void **)&p_3d, size * sizeof(double));
        cudaMalloc((void **)&ppvard, ppvar.size() * sizeof(int));
        ppvar0d = (int *)(&ppvard[0]+0*chunk());
        ppvar1d = (int *)(&ppvard[0]+1*chunk());
        ppvar2d = (int *)(&ppvard[0]+2*chunk());
        cudaMalloc((void **)&pion_datad, ion_data.size() * sizeof(double));
        pion_data0d = (double *)(&pion_datad[0]+0*chunk());
        pion_data1d = (double *)(&pion_datad[0]+1*chunk());
        pion_data2d = (double *)(&pion_datad[0]+2*chunk());
        cudaMalloc((void **)&pVEC_Vd, size * sizeof(double));
        cudaMalloc((void **)&pVEC_RHSd, size * sizeof(double));
        cudaMalloc((void **)&pnid, size * sizeof(int));

        // Data transfers host to device
        cudaMemcpy(p_0d, p_0, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(p_1d, p_1, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(p_2d, p_2, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(p_3d, p_3, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(ppvard, ppvar0, ppvar.size() * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(pion_datad, pion_data0, ion_data.size() * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemset(pion_data1d, 0, size * sizeof(double));
        cudaMemset(pion_data2d, 0, size * sizeof(double));
        cudaMemcpy(pVEC_Vd, pVEC_V, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(pVEC_RHSd, pVEC_RHS, size * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(pnid, pni, size * sizeof(int), cudaMemcpyHostToDevice);

        // Kernel computation
        Na_current_wrapper(p_0d, p_1d, p_2d, p_3d, ppvar0d, ppvar1d, ppvar2d, pion_data0d, pion_data1d, pion_data2d, pVEC_Vd, pVEC_RHSd, pnid, size);

        // Data transfers device to host
        cudaMemcpy(p_3, p_3d, size * sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(pion_data0, pion_datad, ion_data.size() * sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(pVEC_V, pVEC_Vd, size * sizeof(double), cudaMemcpyDeviceToHost);
        cudaMemcpy(pVEC_RHS, pVEC_RHSd, size * sizeof(double), cudaMemcpyDeviceToHost);

        // Free GPU memory
        cudaFree(p_0d);
        cudaFree(p_1d);
        cudaFree(p_2d);
        cudaFree(p_3d);
        cudaFree(ppvard);
        cudaFree(pion_datad);

        cudaError_t cuerr = cudaGetLastError();
        if (cuerr != cudaSuccess)
            std::cout << "WARNING: CUDA Error found " << cudaGetErrorString(cuerr) << std::endl;

#if CHECK_CPU
        /************* CPU VERIFICATION ****************/
        double _rhs, _g, _v;

        for (int i = 0; i < size; i++) {
          //if (i % 2 == 0)
            NA_CURRENT(p_0, p_1, p_2, p_3cpu, ppvar0, ppvar1, ppvar2, pion_data0cpu, pion_data1cpu, pion_data2cpu, pVEC_Vcpu, pVEC_RHScpu, pni, i);
        }
        //for (int i = 0; i < size; i++) {
        //  if (i % 2 == 1)
        //    NA_CURRENT(p_0, p_1, p_2, p_3cpu, ppvar0, ppvar1, ppvar2, pion_data0cpu, pion_data1cpu, pion_data2cpu, pVEC_Vcpu, pVEC_RHScpu, pni, i);
        //}

        // Double-check CPU
        for (int i = size-1; i >= 0; i--) {
            NA_CURRENT(p_0, p_1, p_2, p_3cpu2, ppvar0, ppvar1, ppvar2, pion_data0cpu2, pion_data1cpu2, pion_data2cpu2, pVEC_Vcpu2, pVEC_RHScpu2, pni, i);
        }
        int err0 = 0, err2 = 0, err3 = 0, err4 = 0, err5 = 0;
        for (int i = 0; i < size; i++) {
            if (fabs(p_3[i] - p_3cpu[i]) > 1e-16) {
                err0++;
                std::cout << "Expected: " << p_3cpu[i] << " but found: " << p_3[i] << std::endl;
            }
            if (fabs(pVEC_V[i] - pVEC_Vcpu[i]) > 1e-16) {
                err2++;
                std::cout << "Expected: " << pVEC_Vcpu[i] << " but found: " << pVEC_V[i] << std::endl;
            }
            if (fabs(pVEC_RHS[i] - pVEC_RHScpu[i]) > 1e-16) err3++;
        }
        for (int i = 0; i < ion_data.size(); i++) {
            if (fabs(pion_data0[i] - pion_data0cpu[i]) > 1e-16) {
                std::cout << "[" << i << "] Expected: " << pion_data0cpu[i] << " but found: " << pion_data0[i] << std::endl;
                double eps1 = fabs(pion_data0[i] - pion_data0cpu[i]);
                double eps2 = eps1;
                eps1 = eps1 / fabs(pion_data0[i]);
                eps2 = eps2 / fabs(pion_data0cpu[i]);
                printf("CPU1: %.16f\nCPU2: %.16f\nGPU : %.16f\n    epsilon1 is %g (%.20f) ; epsilon2 is %g (%.20f)\n", pion_data0cpu[i], pion_data0cpu2[i], pion_data0[i], eps1, eps1, eps2, eps2);
                err4++;
            }
        }
        if (err0 != 0)
            std::cout << "[Na current()] " << err0 << " errors found in p_3!" << std::endl;
        if (err2 != 0)
            std::cout << "[Na current()] " << err2 << " errors found in pVEC_V!" << std::endl;
        if (err3 != 0)
            std::cout << "[Na current()] " << err3 << " errors found in pVEC_RHS!" << std::endl;
        if (err4 != 0)
            std::cout << "[Na current()] " << err4 << " errors found in pion_data!" << std::endl;
        if (err5 != 0)
            std::cout << "[Na current()] " << err5 << " errors found in pion_data2!" << std::endl;

        if (err0 == 0 && err2 == 0 && err3 == 0 && err4 == 0 && err5 == 0)
            std::cout << "Na current() GPU computation verified" << std::endl;
        /*****************************/
#endif
    }
};
