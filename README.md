# neuromapp
The Neuronm(ini)app(lication)library reproduces the algorithms of the main software of the BBP as a collection of mini-apps
For its first release, the Neuromapp framework focuses on CoreNeuron application.

The framework is organised under the form

## neuronmapp/app

This directory neuromapp contains the main program (C++) that organises all the mini
applications. A specific README is added, it gives directives for the plug
of new applications.

## neuronmapp/coreneuron_1.0

This directory contains miniapps (C)  about the main behaviour of first release
of coreneuron, 3 miniapps are available: kernel and solver

* kernel: This miniapp extracts the computation kernel of CoreNeuron, the miniapp focuses
	 	 on the vectorization by the compiler
* solver: This miniapp extracts the linear algbra solver of CoreNeuron, the Hines solver 
	         (similar to Thomas Solver for 3 bands matrix)
* cstep: It combines the kernel and the solver to mimic a step time of neuron simulator

* queue: This miniapp simulates the queueing system of CoreNeuron.
 
##hello

This directory provides an example of how could be design a miniapp (C++)

You can disable the compilation of this mini-app by using the following variable in cmake
command: '-DNEUROMAPP_DISABLE_HELLO=TRUE'

## neuromapp/iobench

This directory contains a miniapp that creates a fake report in a key/value fashion and 
stores and reads it from a key/value backend. Several parameters can be tuned, like the 
size of keys and values, the amount of k/v pairs, parallelism (MPI/OMP), randomness of 
reads and writes, etc. This miniapp is similar to the keyvalue miniapp, but in this case 
the parameter configuration is more flexible.

For more information, run the mini-app with the --help argument.

You can disable the compilation of this mini-app by using the following variable in cmake
command: '-DNEUROMAPP_DISABLE_IOBENCH=TRUE'

## neuromapp/keyvalue

This directory contains a miniapp that mimics CoreNeuron simulation loop but outputs 
the results in a key/value fashion. This miniapp is similar to the iobench miniapp, but 
in this case the options are more restrict to a BG/Q environment: three different use 
cases are defined in order to fill 25%, 50% or 75% of the main memory of a BG/Q node.

For more information, run the mini-app with the --help argument.

You can disable the compilation of this mini-app by using the following variable in cmake
command: '-DNEUROMAPP_DISABLE_KEYVALUE=TRUE'

## neuromapp/replib

This directory contains a miniapp that mimics the behavior of Neuron's ReportingLib. 
It uses MPI I/O collective calls to write a fake report to a shared file. The miniapp 
provides several options to distribute data across ranks in different ways.

For more information, run the mini-app with the --help argument.

You can disable the compilation of this mini-app by using the following variable in cmake
command: '-DNEUROMAPP_DISABLE_REPLIB=TRUE'

## neuromapp/nest/synapse

This directory contains a miniapp that simulates synapse models from NEST.

You can disable the compilation of this mini-app by using the following variable in cmake
command: '-DNEUROMAPP_DISABLE_NEST=TRUE'


# Installation Instructions #

1) Create a build directory, for example inside the source directory (```NEUROMAPP_SRC_DIR```):
```
$ mkdir $NEUROMAPP_SRC_DIR/build
```

2) Configure, compile and install the framework. Optionally, specify the desired 
installation directory (```NEUROMAPP_INST_DIR```):
```
$ cd $NEUROMAPP_SRC_DIR/build
$ cmake .. -DCMAKE_INSTALL_PREFIX=$NEUROMAPP_INST_DIR
$ make
$ make install
```

If you're using GCC 5.1 and above, please add the following flag to your ```cmake``` 
command to tell GCC to not use the C++11 ABI:
```
-DCMAKE_CXX_FLAGS=-D_GLIBCXX_USE_CXX11_ABI=0
```

## Dependency Handling

Mandatory dependencies are:
* BOOST >= 1.44
* HDF5
* SLURM
* MPI

The following variables can be set in the ```cmake``` command to disable the compilation of certain 
parts of the framework or ignore certain external libraries that would be used otherwise:

'```-DNEUROMAPP_DISABLE_HDF5MAPP=TRUE```': Ignore the code under neuromapp/hdf5

'```-DNEUROMAPP_DISABLE_HELLO=TRUE```': Ignore the code under neuromapp/hello

'```-DNEUROMAPP_DISABLE_IOBENCH=TRUE```': Ignore the code under neuromapp/iobench

'```-DNEUROMAPP_DISABLE_KEYVALUE=TRUE```': Ignore the code under neuromapp/keyvalue

'```-DNEUROMAPP_DISABLE_NEST=TRUE```': Ignore the code under neuromapp/nest

'```-DNEUROMAPP_DISABLE_REPLIB=TRUE```': Ignore the code under neuromapp/replib

'```-DNEUROMAPP_DISABLE_CASSANDRA=TRUE```': Do not try to find the Cassandra installation

'```-DNEUROMAPP_DISABLE_CEPH=TRUE```': Do not try to find the CEPH installation

'```-DNEUROMAPP_DISABLE_LEVELDB=TRUE```': Do not try to find the LevelDB installation

'```-DNEUROMAPP_DISABLE_SKV=TRUE```': Do not try to find the IBM SKV installation

