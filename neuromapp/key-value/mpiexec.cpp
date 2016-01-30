// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "key-value/utils/tools.h"
#include "key-value/benchmark.h"


typedef keyvalue::meta meta_type;

int toto(int argc, char* argv[]) {
    KeyValueBench<keyvalue::map> bench;
    
    argument a;
    
	argvs args;

	bench.parseArgs(argc, argv, args);
    
    // build the bench infunction of the arguement
    benchmark<meta_type> b(args);
    //bench
    run_loop(b);
    
    double bb = 2;
    
    bb /= 1;
    


//    KeyValueBench<keyvalue::map> bench;
//	argvs args;
//	stats stats;

//	bench.parseArgs(argc, argv, args);

//	bench.run(args, stats);
//
//    std::cout <<  mapp::mpi_filter_master() << "Overall performance ("
//              <<  keyvalue::mpi::master.size() << "):" << std::endl
//              << "  I/O: " << stats.mean_iops() << " kIOPS" << std::endl
//              << "  BW: " << stats.mean_mbw() << " GB/s" << std::endl;
//
//    std::cout << "IOMAPP," << keyvalue::mpi::master.size()  << "," << bench.getNumThreads()
//              << "," << args.usecase() << "," << args.st() << "," << args.md()
//              << "," << args.dt() << "," << args.cg() << "," << args.backend()
//              << "," << ( args.async() ? "async" : "sync" )<< ","<< std::fixed << stats.mean_iops()
//              << "," << stats.mean_mbw() << std::endl;

	return 0;
}
