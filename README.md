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

## neuromapp/nest/synapse

This directory contains a miniapp that simulates synapse models from NEST.
