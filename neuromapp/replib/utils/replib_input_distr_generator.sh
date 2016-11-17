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
#bytes=102400 # 100 KB
#bytes=409600 # 400 KB
bytes=665600 # 650 KB --> Close to a real use case
#bytes=1048576 # 1 MB
#bytes=4194304 # 4 MB # Not working on BG/Q: error @ set fileview
#bytes=16777216 # 16 MB
#bytes=67108864 # 64 MB
#bytes=419430400 # 400 MB
#bytes=536870912 # 512 MB
#bytes=4294967296 # 4 GB
#bytes=5368709120 # 5 GB --> replib hangs...


if [ "$1" = "-h" ]
then
    echo "Usage: "$0" [#ranks] > input_distribution_file.csv"
    echo "    This script generates a file that replib mini-app can read as an input"
    echo "    distribution file. Edit the 'bytes' variable inside the script to set the"
    echo "    desired size written by each rank. Please, redirect the output to a file."
    echo "    Running this script without parameters will generate the file for a"
    echo "    single rank. All ranks will write exactly the same block size."
    echo "    Use '#ranks' (integer) to set the desired number of ranks to generate"
    echo "    the input distribution file."
    echo "    Use '"$0" -h' to show this help and exit."

    exit 
fi

if [ "$#" -eq 1 ]
then
   rank=$1
fi

# Adjust rank for sequence in [0..$rank)
rank=$(($rank - 1))

for i in $(seq 0 $rank)
do
    echo $i",2,"$bytes","$disp
done

