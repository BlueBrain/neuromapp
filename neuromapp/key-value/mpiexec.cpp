// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "key-value/mpi/tools.h"
#include "key-value/mpikey-value.h"

int toto(int argc, char* argv[]) {

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    KeyValueBench<keyvalue::map> bench(rank, size);
	KeyValueArgs args;
	KeyValueStats stats;

	bench.parseArgs(argc, argv, args);

	bench.run(args, stats);

    std::cout <<  mapp::mpi_filter_master() << "Overall performance ("
              << size << " " << (size == 1? "process" : "processes") << "):" << std::endl
              << "  I/O: " << stats.mean_iops() << " kIOPS" << std::endl
              << "  BW: " << stats.mean_mbw() << " GB/s" << std::endl;

    std::cout << "IOMAPP," << size << "," << bench.getNumThreads() << "," << args.usecase() << "," << args.st() << "," << args.md() << "," << args.dt() << ","
               << args.cg() << "," << args.backend() << "," << ( args.async() ? "async" : "sync" )<< ","<< std::fixed << stats.mean_iops() << ","
				<< stats.mean_mbw() << std::endl;

	return 0;
}
