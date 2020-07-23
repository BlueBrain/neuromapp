#!/bin/bash -l
#SBATCH --job-name=h5benchmark_wscaling
#SBATCH --nodes=1
#SBATCH --time=00:21:01
#SBATCH --exclusive
#SBATCH --constraint=cpu
#SBATCH --mem=0
#SBATCH --account=proj16

USER_DIR=/gpfs/bbp.cscs.ch/data/scratch/proj16/srivas
H5BMARK_DIR=$USER_DIR/neuromapp/neuromapp/h5benchmark
DARSHAN_DIR=$USER_DIR/darshan-3.2.1/darshan-runtime/build
DARSHAN_DIR_OUTPUT=$DARSHAN_DIR/darshan-logs/2020/7/23

module load archive/2020-06 hpe-mpi/2.21 hdf5/1.10.5

mpicc -std=c99 -I$HDF5_ROOT/include -L$HDF5_ROOT/lib \
      $H5BMARK_DIR/h5benchmark.c -lhdf5 -o $H5BMARK_DIR/h5benchmark

export OMP_NUM_THREADS=1

for bmark in 0 1
do
    for nranks in 1 2 4 8 16 32 64
    do
        srun -n $nranks env LD_PRELOAD=$DARSHAN_DIR/lib/libdarshan.so \
             $H5BMARK_DIR/h5benchmark $bmark 1 $USER_DIR/morphologies.h5
        
        mv `ls $DARSHAN_DIR_OUTPUT/*_h5benchmark_*` \
            $DARSHAN_DIR_OUTPUT/h5benchmark_$bmark\_1_n$nranks.darshan
    done
done
