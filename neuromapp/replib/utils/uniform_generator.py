#! /bin/env python
import argparse
import numpy as np

def main (args):
  total_blocks        = args.nb_blocks[0] * args.ranks[0]
  avg_block_size      = args.data_size[0] / total_blocks
# a regular distribution of block size and ranks over blocks
  blocks_size         = [avg_block_size] * total_blocks
  blocks_offset       = [avg_block_size*i for i in range(total_blocks)]
  blocks_distrib      = range(total_blocks)

# shake it babe
  if args.rand_pos:
    np.random.shuffle(blocks_distrib)
  min_block_size      = avg_block_size - args.block_dev[0]
  max_block_size      = avg_block_size + args.block_dev[0]
  start = 0;
  for i in range (int(total_blocks)):
    blocks_offset[i] = start
    block_size = np.random.random_integers(min_block_size, max_block_size)
    if (start + block_size > args.data_size[0]):
      block_size = args.data_size[0] - start
    blocks_size[i]   = block_size
    start += block_size

# we want to always get the exact amount of data specified, 
# so lets fill the last block with remaining (from random or not well rounded)
  if (blocks_size[-1]+blocks_offset[-1] < args.data_size[0]):
    blocks_size[-1] = args.data_size[0] - blocks_offset[-1]

# print on output
  for i in range (args.ranks[0]):
    my_first_block  = i*args.nb_blocks[0];
    my_last_block   = my_first_block + args.nb_blocks[0];
    my_blocks       = blocks_distrib[my_first_block:my_last_block]
    my_sizes        = [ blocks_size   [j] for j in my_blocks ]
    my_offsets      = [ blocks_offset [j] for j in my_blocks ]
    sizes_str       = ",".join(str(x) for x in my_sizes)
    offsets_str     = ",".join(str(x) for x in my_offsets)
    print ("{rank},{blocks},{sizes},{offsets}".format (rank=i, blocks=len(my_sizes)+1, sizes=sizes_str, offsets=offsets_str))

if __name__ == "__main__":
  parser = argparse.ArgumentParser(prog='PROG', description='%(prog)s [options]')
  parser.add_argument('ranks',         type=int,  nargs=1,                 help='number of ranks')
  parser.add_argument('data_size',     type=int,  nargs=1,                 help='total size of data')
  parser.add_argument('--nb_blocks',   type=int,  nargs=1, default=[1],    help='number of blocks per rank')
  parser.add_argument('--block_dev',   type=int,  nargs=1, default=[0],    help='variation of block size')
  parser.add_argument('--rand_pos',    type=bool, nargs=1, default=True, help='randomize blocks position')
  main (parser.parse_args())
