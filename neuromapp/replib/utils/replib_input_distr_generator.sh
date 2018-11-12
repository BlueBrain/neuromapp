#!/bin/sh

# Generate the appropriate file so that replib mini-app can understand a report distribution
# Please, redirect the output to a file

# File format is:
# rankID, #chunks+1, CSV_list_of_chunk_sizes, CSV_list_of_chunk_offsets

# In this case, we only support the writing of 1 single block per process
# Please, note that we're not calculating offsets: the mini-app will do it for us
# This means that ranks will write sequentially, one after the other, in the file
disp=0

# Set the number of ranks
rank=1

# Set the result size of one reporting time step
# That is, total size written per rank at each MPI I/O call
# Please note that the maximum value must be under 40 GB (due to integer overflow)
# and that not all combinations work on all types of systems
# Here are some examples:
#bytes=65536 # 64 KB
#bytes=102400 # 100 KB
#bytes=131072 # 128 KB
#bytes=256000 # 250 KB
#bytes=262144 # 256 KB
#bytes=409600 # 400 KB
#bytes=524288 # 512 KB
bytes=665600 # 650 KB --> Close to a real use case
#bytes=1048576 # 1 MB
#bytes=4194304 # 4 MB # Not working on BG/Q: error @ set fileview
#bytes=8388608 # 8 MB
#bytes=16777216 # 16 MB
#bytes=33554432 # 32 MB
#bytes=67108864 # 64 MB
#bytes=134217728 # 128 MB
#bytes=268435456 # 256 MB
#bytes=419430400 # 400 MB
#bytes=536870912 # 512 MB
#bytes=4294967296 # 4 GB
#bytes=5368709120 # 5 GB --> replib hangs...

#bytes=9984000 # 9750 KB ~10 MB (650 KB * 15, #neurons fitting 64 GB DRAM)
#bytes=19968000 # 19500 KB ~19 MB (650 KB * 27)
#bytes=26624000 # 26000 KB ~25 MB (650 KB * 40, if we have 6 MB DRAM / neuron)
#bytes=53248000 # 52000 KB ~51 MB (650 KB * 80)
#bytes=85196800 # 83200 KB ~81 MB (650 KB * 512 nodes * 16 ranks/node --> data generated by 1 midplane of BG/Q)


if [ "$1" = "-h" ]
then
    echo "Usage: "$0" [#ranks] [#bytes] > input_distribution_file.csv"
    echo "  #ranks:   Integer, sets the number of ranks to generate the input"
    echo "            distribution file."
    echo "  #bytes:   Integer, sets the number of bytes to write by each rank."
    echo "            distribution file."
    echo "  This script generates a file that replib mini-app can read as an input"
    echo "  distribution file. Use the parameters mention above to tune the number"
    echo "  of ranks and the amount of data written by each rank. Please, redirect"
    echo "  the output to a file. Running this script without parameters will"
    echo "  generate the file for a single rank writing a block of 650 KB. All ranks"
    echo "  will write exactly the same block size."
    echo "  Use '"$0" -h' to show this help and exit."

    exit 
fi

if [ "$#" -gt 0 ]
then
   rank=$1
fi
if [ "$#" -gt 1 ]
then
   bytes=$2
fi

# Adjust rank for sequence in [0..$rank)
rank=$(($rank - 1))

for i in $(seq 0 $rank)
do
    echo $i",2,"$bytes","$disp
done

