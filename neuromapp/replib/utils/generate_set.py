#!/usr/bin/env python
import subprocess
nb_procs      = [16, 32, 128, 256, 512]
size_per_rank = [4000, 8000, 16000, 4000000, 8000000]
nb_blocks     = [1,8,32,64]

generators    = ["./uniform_generator.py {ranks} {total_size} --nb_blocks {nb_blocks} > distributions/{rank}_{size}_{nb_blocks}.distrib".
    format(ranks=ranks, total_size=ranks*size, size=size, nb_blocks=blocks)
    for ranks in nb_procs
    for size in size_per_rank
    for blocks in nb_blocks]

for gen in generators :
  subprocess.exec (gen)
