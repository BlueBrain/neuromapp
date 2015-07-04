CC = icc
CPU_CFLAGS = -g -O3 -Minfo 

CC = pgcc
GPU_CFLAGS = -O2 -ta=nvidia -Minfo=accel  -acc

all: soa_vs_aos_test

soa_vs_aos_test: soa_vs_aos_test.c time.c 
	 ${CC} $(GPU_CFLAGS) soa_vs_aos_test.c -o $@_gpu
	 ${CC} $(CPU_CFLAGS) soa_vs_aos_test.c -o $@_cpu

clean:	
	rm -f  soa_vs_aos_test soa_vs_aos_test_gpu soa_vs_aos_test_cpu *.o

