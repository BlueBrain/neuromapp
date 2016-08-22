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
	 
## neuromapp/queuing
	 
This directory contains a miniapp that simulates the queueing system of CoreNeuron.

Moreover the hello directory provides informations to create a new miniapp

##hello

This directory provides an example of how could be design a miniapp (C++)

## neuromapp/keyvalue

This directory contains a miniapp that mimics CoreNeuron simulation loop but outputs 
the results in a key/value fashion. Three different use cases are defined in order 
to fill 25%, 50% or 75% of the main memory of a BG/Q node.

For more information, run the mini-app with the --help argument.

## neuromapp/replib

This directory contains a miniapp that mimics the behavior of Neuron's ReportingLib. 
It uses MPI I/O collective calls to write a fake report to a shared file. The miniapp 
provides several options to distribute data across ranks in different ways.

For more information, run the mini-app with the --help argument.

