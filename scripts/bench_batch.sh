#!/bin/bash
#
#SBATCH --job-name="benchmarking adios"
#SBATCH --time=01:00:00
#SBATCH --mail-user=jeremy.fouriaux@epfl.ch
#SBATCH --mail-type=ALL
#SBATCH --partition=test
#SBATCH --nodes=1
#SBATCH --ntasks=16
#SBATCH --ntasks-per-node=16
#SBATCH --output=benchmarkingADIOS.%j.o
#SBATCH --error=benchmarkingADIOS.%j.e
#SBATCH --account=proj16
#SBATCH --exclusive 
 
#bug fix: for partitions >1024 mpi ranks, this will cause an issue on the output unless this flag is past:
export  BGLOCKLESSMPIO_F_TYPE=0x47504653
 
#======START=====
module load slurm
module load hdf5/1.8.15-patch1 zlib/1.2.3 bg-xl
module load boost/1.54.0-bgqpatch
moduel load nix/bgq/python/2.7-full
cd ../install_blue/bin
./bench_replib2.py
