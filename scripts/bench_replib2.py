#! /usr/bin/env python
#  ____  _____ _   _  ____ _   _      _ _ _                     #
# | __ )| ____| \ | |/ ___| | | |    | (_) |__  _ __ ___ _ __   #
# |  _ \|  _| |  \| | |   | |_| |    | | | '_ \| '__/ _ \ '_ \  #
# | |_) | |___| |\  | |___|  _  |    | | | |_) | | |  __/ |_) | #
# |____/|_____|_| \_|\____|_| |_|    |_|_|_.__/|_|  \___| .__/  #
#                                                      |_|      #

import subprocess
import re
import sys
backends    = ['mpiio','adios']
# we test from 16 procs to 512
#nb_procs    = [2**e for e in range(4,9)]
# number of different blocks written by the procesors
#nb_blocks   = [1, 8, 32, 64]
#load_sizes  = ["4000", "8000", "16000", "4000000", "8000000"]
#block_place = ["regular", "shuffle"]
#block_size  = ["same", "diffs"]
#models      = ["{blk_place}_{blk_size}_{nb_blk}_{load_size}".format()
#      for blk_place in block_place
#      for blk_size  in block_size
#      for nb_blk    in nb_blocks
#      for load_size in load_sizes
#              ]

nb_procs    = [16]
nb_blocks   = [1,8]
load_sizes  = [4000]
models      = ["{load_size}_{nb_blk}".format(load_size=load_size, nb_blk=nb_blk)
                for nb_blk    in nb_blocks
                for load_size in load_sizes
              ]
distributions = { key: ["{nb_proc}_{model}.distrib".format(nb_proc=key, model=model) for model in models] for key in nb_procs}

output_filename = "/gpfs/bbp.cscs.ch/scratch/gss/bgq/fouriaux/currents.bbp"
exec_name = "/gpfs/bbp.cscs.ch/home/fouriaux/Devel/adios_dev/neuromapp/install_blue/bin/MPI_Exec_rl"
srun_name = "srun"
avg_re    = re.compile(r'Average bandwidth: ([-+]?\d*\.\d+|\d+)')
max_re    = re.compile(r'Max bandwidth: ([-+]?\d*\.\d+|\d+) MB/s writing ([-+]?\d*\.\d+|\d+)')
min_re    = re.compile(r'Min bandwidth: ([-+]?\d*\.\d+|\d+) MB/s writing ([-+]?\d*\.\d+|\d+)')
owc_re    = re.compile(r'Open\+write\+close aggregated bandwidth: ([-+]?\d*\.\d+|\d+) MB/s')
file_size = re.compile(r'\d+')
print ("backend; nb_proc;\tAvg Bandwith (per rank);\tAvg Open+Write+Close;\tMin (MB/s);\tMax (MB/s);\tMin (Kb);\tMax (Kb);\tNb Cells;\tFile size;\t ranks_size_blocks")
for run in range (2):
  for backend in backends:
    for nb_proc in nb_procs:
      for distrib in distributions[nb_proc]:
        subprocess.call (["rm", "-f", output_filename])
        command = "{srun} -n {nbproc} --exclusive {replib} -w fileNb -f distributions/{distribution} -b {backend} -s 100 -r 10 -o {output}".format(srun=srun_name, replib=exec_name, nbproc=nb_proc, distribution=distrib, backend=backend, output=output_filename)
        print (command)
        output = subprocess.check_output(command.split())
        print (output)
        m_avg = 0
        n_min = 0
        m_max = 0
#        try :
        m_avg       = avg_re.search(output).group(1)
        m_min       = min_re.search(output).group(1)
        m_max       = max_re.search(output).group(1)
        m_max_kB    = max_re.search(output).group(2)
        m_min_kB    = min_re.search(output).group(2)
        m_owc       = owc_re.search(output).group(1)
        du_output   = subprocess.check_output ("du {}".format(output_filename).split())
        final_size  = file_size.search(du_output).group(0)
        print ("{};{:>7};{:>31};{:>18};{:>15};{:>13};{:>15};{};{};{}".format(backend, nb_proc, m_avg, m_owc, m_min, m_max, m_min_kB, m_max_kB, final_size, distrib))
#        except :
#           print (sys.exc_info()[0])
#          print ("{};{:>7};{:>31};{:>18};{:>15};{:>13};{:>15};{};{}".format(backend, nb_proc, "---", "---", "---", "---", "---","---", "---", distrib))
