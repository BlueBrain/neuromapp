#!/bin/bash -l
#
#SBATCH --job-name="benchmarking"
#SBATCH --time=00:04:00
#SBATCH --nodes=1
#SBATCH --gres=gpu:1
#SBATCH --partition=normal
#SBATCH --output=benchmarking.%j.o
#SBATCH --error=benchmarking.%j.e

#======START=====
#module purge
module load slurm

# nvprof
export COMPUTE_PROFILE=1
# disable PGI profiling
export PGI_ACC_TIME=0

export OMP_NUM_THREADS=1
EXEC=../bin/exe_cray_openacc

aprun -n 1 -d $OMP_NUM_THREADS $EXEC

