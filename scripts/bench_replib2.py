#! /usr/bin/env python3
#  ____  _____ _   _  ____ _   _      _ _ _                     #
# | __ )| ____| \ | |/ ___| | | |    | (_) |__  _ __ ___ _ __   #
# |  _ \|  _| |  \| | |   | |_| |    | | | '_ \| '__/ _ \ '_ \  #
# | |_) | |___| |\  | |___|  _  |    | | | |_) | | |  __/ |_) | #
# |____/|_____|_| \_|\____|_| |_|    |_|_|_.__/|_|  \___| .__/  #
#                                                      |_|      #

import subprocess
import re
backends    = ['mpiio','adios']
# we test from 16 procs to 512
nb_procs    = [2**e for e in range(4,9)]
# number of different blocks written by the procesors
nb_blocks   = [1, 8, 32, 64]
load_sizes  = ["4k", "8k", "16k", "4M", "8M"]
block_place = ["regular", "shuffle"]
block_size  = ["same", "diffs"]
models      = ["{blk_place}_{blk_size}_{nb_blk}_{load_size}".format()
      for blk_place in block_place
      for blk_size  in block_size
      for nb_blk    in nb_blocks
      for load_size in load_sizes
              ]
distributions = { key: ["{nb_proc}_{model}".format(nb_proc=key, model=model) for model in models] for key in nb_procs}

output_filename = "/gpfs/bbp.cscs.ch/scratch/gss/viz/fouriaux/currents.bbp"
exec_name = "/gpfs/bbp.cscs.ch/home/fouriaux/Devel/adios_dev/neuromapp/install/bin/MPI_Exec_rl"
srun_name = "/usr/bin/srun"
avg_re    = re.compile(r'Average bandwidth: ([-+]?\d*\.\d+|\d+)')
max_re    = re.compile(r'Max bandwidth: ([-+]?\d*\.\d+|\d+) MB/s writing ([-+]?\d*\.\d+|\d+)')
min_re    = re.compile(r'Min bandwidth: ([-+]?\d*\.\d+|\d+) MB/s writing ([-+]?\d*\.\d+|\d+)')
file_size = re.compile(r'\d+')
print ("backend; nb_proc;\tAvg Bandwith (per rank);\tMin (MB/s);\tMax (MB/s);\tMin (Kb);\tMax (Kb);\tNb Cells;\tFile size")
for run in range (1):
  for backend in backends:
    for nb_proc in nb_procs:
      for distrib in distributions[nb_proc]:
        subprocess.getoutput (["rm", output_filename])
        output = subprocess.getoutput("{srun} -n {nbproc} -w fileNb -f {distribution} {replib} -b {backend} -s 100 -r 10 -o {output}".format(srun=srun_name, replib=exec_name, nbproc=nb_proc, distribution=distrib, backend=backend, output=output_filename))
        m_avg = 0
        n_min = 0
        m_max = 0
        try :
          m_avg       = avg_re.search(output).group(1)
          m_min       = min_re.search(output).group(1)
          m_max       = max_re.search(output).group(1)
          m_max_kB    = max_re.search(output).group(2)
          m_min_kB    = min_re.search(output).group(2)
          du_output   = subprocess.getoutput ("du {}".format(output_filename))
          final_size  = file_size.search(du_output).group(0)
          print ("{};{:>7};{:>31};{:>18};{:>15};{:>13};{:>15};{:>15};{}".format(backend, nb_proc, m_avg, m_min, m_max, m_min_kB, m_max_kB, nb_cells, final_size))
        except:
          print ("{};{:>7};{:>31};{:>18};{:>15};{:>13};{:>15};{:>15};{}".format(backend, nb_proc, "---", "---", "---", "---","---", nb_cells, "---"))
