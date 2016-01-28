# neuromapp
The Neuronm(ini)app(lication)library reproduces the algorithms of the main software of the BBP as a collection of mini-apps
For its first release, the Neuromapp framework focuses on CoreNeuron application.

The framework is organised under the form

	- neuronmapp/app

This directory neuromapp contains the main program (C++) that organises all the mini
applications. A specific README is added, it gives directives for the plug
of new applications.

	- neuronmapp/coreneuron_1.0

This directory contains miniapps (C)  about the main behaviour of first release
of coreneuron, 2 miniapps are available: kernel and solver

	 kernel: This miniapp extracts the computation kernel of CoreNeuron,
                the miniapp focus on the vectorization by the compiler
         solver: This miniapp extracts the linear algbra solver of CoreNeuron,
                the Hines solver (similar to Thomas Solver for 3 bands matrix)

	 - hello

This directory provides an example of how could be design a miniapp (C++)

        - queuing:

This miniapp simulates the queueing system of CoreNeuron.
