simd:
	mkdir -p bin
	g++ -O3 -I . main.cpp -o bin/exe_simd

cray-simd:
	mkdir -p bin
	CC -O3 -hlist=a -h vector3 -I . main.cpp -o bin/exe_cray_simd

cray-openacc:
	# Link to nvidia library only on piz daint at the moment 
	mkdir -p bin
	CC -D__OPENACC__ -h msgs -O3 -hlist=a -h vector3 -h pragma=acc -h nopragma=omp -fpic -dynamic -lcudart -G1 -I . main.cpp -o bin/exe_cray_openacc 

pgi-openacc:
	mkdir -p bin
	pgc++ -D__OPENACC__ -Minfo=accel -mp=nonuma -fast -acc -ta=nvidia,time -I . main.cpp -o bin/exe_pgi_openacc

all: simd cray-simd pgi-openacc cray-openacc 

clean:
	rm -rf bin main.lst exe_simd exe_cray_simd exe_pgi_openacc exe_cray_openacc main_1.cub  main_1.ptx

