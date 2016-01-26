// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "key-value/mpikey-value.h"

int toto(int argc, char* argv[]) {

    MPI::Init();

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    KeyValueBench<keyvalue::map> bench(rank, size);
	KeyValueArgs args;
	KeyValueStats stats;

	bench.parseArgs(argc, argv, args);

	MPI::COMM_WORLD.Barrier();

	bench.run(args, stats);

	MPI::COMM_WORLD.Barrier();

	std::cout << "Bye bye from " << rank << std::endl;
	std::cout.flush();

    MPI::Finalize();

	if (rank == 0) {
		std::cout << "Overall performance (" << size << " " << (size == 1? "process" : "processes") << "):" << std::endl
				<< "  I/O: " << stats.mean_iops_ << " kIOPS" << std::endl
				<< "  BW: " << stats.mean_mbw_ << " GB/s" << std::endl;

		// CSV output data format: miniapp_name, num_procs, num_threads/proc, usecase, simtime (ms), mindelay (ms), dt (ms),
		// cell_groups, backend, sync/async, iops (kIOP/S), bw (GB/s)
		std::cout << "IOMAPP," << size << "," << bench.getNumThreads() << "," << args.usecase() << "," << args.st() << "," << args.md() << "," << args.dt() << ","
				<< args.cg() << "," << args.backend() << "," << ( args.async() ? "async" : "sync" )<< ","<< std::fixed << stats.mean_iops_ << ","
				<< stats.mean_mbw_ << std::endl;
	}

	return 0;
}
