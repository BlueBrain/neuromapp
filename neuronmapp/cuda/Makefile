all: simd nosimd cuda

simd:
	xlc++ -I. -std=c++11 -qlist -qreport -qhot=simd  -qsimd=auto -q64 -qarch=pwr8 -qtune=pwr8 -qaltivec -qprefetch=aggressive -qsmp=omp \
        -O3 -qpath=IL:/opt/ibm/patches/exe/  -qxflag=SIMDAGGRESSIVE  -qxflag=SIMDLEVEL10 main.cpp -o exe_simd

nosimd:
	xlc++ -I. -std=c++11 -qlist -qreport -qnoenablevmx -qsimd=noauto -q64 -qarch=pwr8 -qtune=pwr8 -qaltivec -qprefetch=aggressive -qsmp=omp -O3 main.cpp -lmass -o exe_nosimd

cuda: cuda2

cuda1:
	nvcc -O3 -m64 -c mechanism/Na.cu -o mechanism/Na_cuda.o -arch=sm_35
	xlc++ -I. -std=c++11 -qlist -qreport -qnoenablevmx -qsimd=noauto -q64 -qarch=pwr8 -qtune=pwr8 -qaltivec -qprefetch=aggressive -O3 main.cpp mechanism/Na_cuda.o -lmass -o exe_cuda -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcudart


cuda2:
	nvcc -O3 -m64 -c mechanism/Na.cu -o mechanism/Na_cuda.o -arch=sm_35
	xlc++ -I. -std=c++11 -O2 main.cpp mechanism/Na_cuda.o -lmass -o exe_cuda -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcudart



clean:
	rm -f exe_simd exe_nosimd exe_cuda

