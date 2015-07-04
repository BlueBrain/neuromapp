#bin/bash
#set -x

function execute
{
    for binary in exe_cuda #exe_simd exe_nosimd
    do
        #OMP_NUM_THREADS=1 XLSMPOPTS=startproc=0:stride=0  numactl -l ./${binary} 1000000
        ./${binary} 1000000
    done
}

execute
