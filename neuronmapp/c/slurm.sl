#!/bin/bash -l
#
#SBATCH --job-name="benchmarking"
#SBATCH --time=00:04:00
#SBATCH --nodes=1
##SBATCH --reservation=eurohack15
#SBATCH --gres=gpu:1
#SBATCH --partition=normal
#SBATCH --output=benchmarking.%j.o
#SBATCH --error=benchmarking.%j.e

#======START=====
#module purge
module load slurm

# nvprof
export PMI_NO_FORK=1
#export COMPUTE_PROFILE=1
# disable PGI profiling
export PGI_ACC_TIME=0

export OMP_NUM_THREADS=1
export CRAY_ACC_DEBUG=1

#echo "Running with pgi"
module unload PrgEnv-cray PrgEnv-pgi
module load PrgEnv-pgi
module load cudatoolkit
EXEC=bin/Na_test_pgi
nvprof --version &> nvprof_version_pgi 
##aprun -n 1 -d $OMP_NUM_THREADS $EXEC 318046
aprun -n 1 -cc none -b nvprof -m all $EXEC 318046
#aprun -n 1 -cc none -b $EXEC 318046

#sleep 10
#echo "Running with cray"
module unload PrgEnv-cray PrgEnv-pgi
module load module craype-accel-nvidia35
module load PrgEnv-cray
EXEC=./bin/Na_test_cray
nvprof --version &> nvprof_version_cray
## give all cores on socket and let run time pin for threads
aprun -n 1 -cc none -b nvprof -m all $EXEC 318046  
#aprun -n 1 -cc none -b $EXEC 318046
