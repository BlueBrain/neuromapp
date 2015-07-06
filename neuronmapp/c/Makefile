
pgi-openacc: 
	mkdir -p bin
	CC -D__OPENACC__ -Minfo=accel -mp=nonuma -fast -acc -ta=nvidia,time -I . Na.c -o bin/Na_test_pgi 

cray-openacc:
	mkdir -p bin
	CC -D__OPENACC__ -h list=a -hacc -O3 -fpic -dynamic -lcudart -G1 -I . Na.c -o bin/Na_test_cray 

all: cray-openacc pgi-openacc

clean:	
	rm -rf Na_1*  bin *.o comp_cray  comp_pgi 

