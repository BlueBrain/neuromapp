simd:
	g++ -O3 -I . main.cpp -o bin/exe_simd

cray-simd:
	CC -O3 -hlist=a -h vector3 -I . main.cpp -o bin/exe_cray_simd

all: simd cray-simd

clean:
	rm -f exe_simd exe_cray_simd

