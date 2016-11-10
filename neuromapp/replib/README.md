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
    for "s" reporting_steps {
        for 10 iterations {
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
* --steps, -s [int]: Number of reporting steps. 1 reporting step happens every 10 
    simulation steps (default, 1)
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
    - Each item contains the information of all cells for one reporting time step:

      | rep_time_step_1 | rep_time_step_2 | rep_time_step_3 | ...

    - Each time step contains the information for each cell. Cells are always ordered 
      in the same way for all time steps, and the order criteria is their GID (integer):

      | cell_1 | cell_2 | cell_3 | ...

    - Each cell contains the information about its cell compartments. There is one float 
      value (voltage) for each compartment and a cell can hold a maximum of 350 
      compartments. When using the ``` --write rnd1b ``` option, the number of 
      compartments per cell is computed randomly with a lower bound of 200 and an upper 
      bound of 350:

      | voltage_comp_1 | voltage_comp_2 | voltage_comp_3 | ...

    - Thus, the write size per process cannot be computed in advance and is reported by 
      the mini-app at the end of the execution (per reporting step).

* If a data distribution file is used to configure the mini-app execution, the amount 
  of cells is then ignored and the block sizes read from the data distribution file 
  are used instead. In this case, the amount of data written per process can be easily 
  derived from the data distribution file and the number of reporting steps.



