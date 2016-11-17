# replib Description #

This mini-app simulates the process of writing a simulation report with ReportingLib 
library, used by Neuron.
Being aware of the characteristics of a real simulation report, it creates random 
data that match these characteristics. Then, MPI I/O collective calls 
(``` MPI_File_write_all() ```) are used to write a single, shared file between all 
the MPI processes. The mini-app reports the average bandwidth achieved in the 
``` MPI_File_write_all() ``` call.

replib is able to write the report using different I/O patterns:
* Each process writes a single, contiguous block of data
* Each process writes a set of non-contiguous blocks of data (interleaved between 
  other processes) 

## replib Steps ##

replib depends on MPI library. The steps performed by the mini-app can be described 
as follows:

1) Initialization

2) Run test:

```
    for "r" reporting_steps {
        for "s" simulation_steps {
            simulate_computing_phase(sleep)
            create_data_in_bufferToFill
        }
        swap_buffers(bufferToFill, bufferToWrite)
        MPI_File_write_all(bufferToWrite)
    }
```

3) Finalization

## replib Parameters ##

This mini-app has several parameters that can be configured in order to tune it. 
Passing the '-h' parameter will print a help message listing the possible 
parameters and a short description.

Here's a more detailed description:
* --help, -h: Produce the help message
* --numproc, -p [int]: Number of MPI processes to use (default, 1)
* --write, -w [string]: Specify how processes write to file. Supported options are:
 * rnd1b: create random blocks of data for each process, write 1 contiguous 
     block per process (default)
 * file1b: read the data distribution (number of blocks, size of each block) from 
     a file, but always write 1 block per process (if multiple blocks are found 
     for a process, join them in a single block)
 * fileNb: read the data distribution (number of blocks, size of each block, 
     offset in report output) from a file, reproduce exactly the same block 
     assignment and placement
* --output, -o [string]: Path and name of the output report file (default 
    "./currents.bbp")
* --file, -f [string]: Path to the file with the data distribution per process 
    (none by default)
* --invert, -i: If present, invert the rank ID when assigning blocks to processes. 
    This option only applies when data distribution is read from a file 
    (``` --write fileXX ``` options) (no invert by default)
* --numcells, -c [int] Number of cells to generate random data. This will decide, 
    with some degree of randomness, the block size of each process. This option 
    only applies when data distribution is generated randomly 
    (``` --write rnd1b ```) options (default, 10).
* --sim-steps, -s [int]: Number of simulation steps for each reporting step 
    (default, 15)
* --rep-steps, -r [int]: Number of reporting steps. 1 reporting step happens every 
    'sim-steps' simulation steps (default, 1)
* --time, -t [int]: Amount of time (in ms) spent in 1 simulation step (default, 100)
* --check, -c: If present, verify the output report was correctly written 
    (no check by default)

## Other Useful Information ##

Some other relevant information for this mini-app:

* The parameters of this mini-app are extremely linked to neuroscientific simulation 
  parameters: number of cells to simulate, amount of reporting time steps, etc. In 
  order to better understand this parameters, the user must be aware of the following 
  concepts:
 * The real report file is structured as an array, where:
    - Each item contains the information of all cells for one simulation time step:

      | sim_time_step_1 | sim_time_step_2 | sim_time_step_3 | ...

    - Each time step contains the information for each cell. Cells are always ordered 
      in the same way for all time steps, and the order criteria is their GID (integer):

      | cell_1 | cell_2 | cell_3 | ...

    - Each cell contains the information about its cell compartments. There is one float 
      value (voltage) for each compartment and a cell can hold a maximum of 350 
      compartments. When using the ``` --write rnd1b ``` option, the number of 
      compartments per cell is computed randomly with a lower bound of 100 and an upper 
      bound of 350:

      | voltage_comp_1 | voltage_comp_2 | voltage_comp_3 | ...

    - Thus, the write size per process cannot be computed in advance and is reported by 
      the mini-app at the end of the execution (per reporting step).

    - In this case, the frequency of writing data to disk depends on the 
      ``` --sim-steps S ``` parameter. ``` S ``` simulation time steps will be grouped 
      together as one single block and will be written to disk at once.

* If a data distribution file is used to configure the mini-app execution, the amount 
  of cells is then ignored and the block sizes read from the data distribution file 
  are used instead. In this case, the amount of data written per process can be easily 
  derived from the data distribution file and the number of reporting steps. In this 
  case, the ``` --sim-steps S ``` parameter is only used to simulate the computing 
  phase. A script is provided for convenience to help the user generate the desired 
  input distribution file (``` $INSTALL_DIR/bin/replib_input_distr_generator.sh ```).

* The expected structure of the data distribution file is the following:
  ``` rankID, #chunks+1, CSV_list_of_chunk_sizes, CSV_list_of_chunk_offsets ```

* Please, make sure that all processes have some data to write. The bandwidth 
  calculation takes into account all the statistics from all processes, including 
  those writing 0 bytes of data (which results in 0 MB/s bandwidth). An easy way to 
  ensure this is to always set the number of cells (``` --numcells ``` parameter) to 
  be at least the number of MPI processes. If using an input distribution file, it 
  should be enough to check that there are at least as many lines as the number of 
  MPI processes.

## Examples ##

Please, note that the mini-app can always be launched from the driver:

```
$ $INSTALL_DIR/bin/app 
Welcome to NeuroMapp! Please enter the name of the miniapp you wish to execute followed by any arguments you wish to pass to it.
Please avoid preceding and trailing whitespace!
To finish type quit
>? replib
Running command: /usr/bin/srun -n 1 [INSTALL_DIR]/bin/MPI_Exec_rl  -w rnd1b -o ./currents.bbp   -c 10 -s 15 -r 1 -t 100
Mini-app configuration:
 procs: 1 
 write_: rnd1b 
 input_dist_:  
 output_report_: ./currents.bbp 
 invert_: 0 
 numcells_: 10 
 sim_steps_: 15 
 rep_steps_: 1 
 elems_per_step_: 48495 
 sim_time_ms_: 100 
 check_: 0 
Average bandwidth: 797.451 MB/s per rank
Aggregated bandwidth: 797.451 MB/s
Max bandwidth: 797.451 MB/s writing 189.434 KB from rank 0
Min bandwidth: 797.451 MB/s writing 189.434 KB from rank 0
RLMAPP,1,rnd1b,seq,10,15,1,797.451182,797.451182,797.451182,193980,0,797.451182,193980,0
```

However, the following examples use the ``` MPI_Exec_rl ``` executable directly. Please, 
beware that parameter checking is ONLY done when running through the driver!

### Random distribution ###

``` $ srun -N 1 -n 8 install.viz/bin/MPI_Exec_rl -w rnd1b -o ./currents.bbp -r 9 -c 10 -s 11 ```

Launch 8 ranks on the same node, using one single, continuous block per process with 
random sizes (``` -w rnd1b ```), run the simulation for 9 reporting steps (``` -r 9 ```), 
each reporting step contains 11 simulation steps (``` -s 11 ```), generate data for 10 
cells (``` -c 10 ```) and write the results in ``` ./currents.bbp ```.

Since the block sizes are generated randomly, we cannot calculate the exact amount of 
data written by each process. However, we can calculate lower and upper bounds:
* Each process generates data for 10 cells that have between 100 and 350 compartments 
  (1 compartment represented by 1 ``` float ```). So, each process will write between 
  4000 bytes and 14000 bytes per simulation step.

* Each reporting step contains 11 simulation steps, so each rank will write between 
  44000 bytes and 154000 bytes per reporting step (this is the amount of data written 
  per rank in one MPI I/O call).

* In total, the mini-app will run for 9 reporting steps, so each rank will write between 
  396000 bytes and 1386000 bytes (evenly distributed in 9 MPI I/O calls).

* Since the mini-app was launched with 8 ranks, the aggregated amount of data written 
  per reporting step (1 MPI I/O call) is between 352000 bytes and 1232000 bytes. Thus, 
  the total amount of data written by all ranks will be between 3168000 and 11088000 
  bytes (the latter corresponds to the output report's file size).


### Using data distribution from file ###

```
$ $INSTALL_DIR/bin/replib_input_distr_generator.sh 8 > data_distr.csv
$ srun -N 1 -n 8 install.viz/bin/MPI_Exec_rl -w file1b -f ./data_distr.csv -o /tmp/currents.bbp -r 7 -s 5
```

Launch 8 ranks on the same node, using one single, continuous block per process using the 
given distribution file (``` -w file1b -f ./data_distr.csv ```), run the simulation for 7 
reporting steps (``` -r 7 ```), each reporting step contains 5 simulation steps 
(``` -s 5 ```), write the results in ``` /tmp/currents.bbp ```.

In this case, we can calculate the exact amount of data written by each process:
* From the data distribution file, we can see the following:
  ```
  $ cat data_distr.csv 
  0,2,665600,0
  1,2,665600,0
  2,2,665600,0
  3,2,665600,0
  4,2,665600,0
  5,2,665600,0
  6,2,665600,0
  7,2,665600,0
  ```
  So, each process is writing 665600 bytes per reporting step. Remember that 
  ``` -s 5 ``` is ignored in this case. So, this is the amount of data written 
  per rank in one MPI I/O call.

* In total, the mini-app will run for 7 reporting steps, so each rank will write 
  4659200 bytes (evenly distributed in 7 MPI I/O calls).

* Since the mini-app was launched with 8 ranks, the aggregated amount of data written 
  per reporting step (1 MPI I/O call) is 5324800 bytes. Thus, the total amount of 
  data written by all ranks will be 37273600 bytes (the latter corresponds to the 
  output report's file size).

