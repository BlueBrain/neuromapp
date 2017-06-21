#! /usr/bin/env python3
#  ____  _____ _   _  ____ _   _      _ _ _                     #
# | __ )| ____| \ | |/ ___| | | |    | (_) |__  _ __ ___ _ __   #
# |  _ \|  _| |  \| | |   | |_| |    | | | '_ \| '__/ _ \ '_ \  #
# | |_) | |___| |\  | |___|  _  |    | | | |_) | | |  __/ |_) | #
# |____/|_____|_| \_|\____|_| |_|    |_|_|_.__/|_|  \___| .__/  #
#                                                      |_|      #

import subprocess
import re
backends = ['mpiio','adios']
output_filename = "/gpfs/bbp.cscs.ch/scratch/gss/viz/fouriaux/currents.bbp"
exec_name = "/gpfs/bbp.cscs.ch/home/fouriaux/Devel/adios_dev/neuromapp/install/bin/MPI_Exec_rl"
srun_name = "/usr/bin/srun"
avg_re    = re.compile(r'Average bandwidth: ([-+]?\d*\.\d+|\d+)')
max_re    = re.compile(r'Max bandwidth: ([-+]?\d*\.\d+|\d+) MB/s writing ([-+]?\d*\.\d+|\d+)')
min_re    = re.compile(r'Min bandwidth: ([-+]?\d*\.\d+|\d+) MB/s writing ([-+]?\d*\.\d+|\d+)')
file_size = re.compile(r'\d+')
print ("backend; nb_proc;\tAvg Bandwith (per rank);\tMin (MB/s);\tMax (MB/s);\tMin (Kb);\tMax (Kb);\tNb Cells;file size")
for run in range (5):
  for backend in backends:
    for nb_proc in [2**e for e in range (4,7)]:
      for nb_cells in [10**e for e in range (4,7)]:
        subprocess.getoutput (["rm", output_filename])
        output = subprocess.getoutput("{srun} -n {nbproc} {replib} -c {nbcell} -b {backend} -s 100 -r 10 -o {output}".format(srun=srun_name, replib=exec_name, nbproc=nb_proc, nbcell=nb_cells, backend=backend, output=output_filename))
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
