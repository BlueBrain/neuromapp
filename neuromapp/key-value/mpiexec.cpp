// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "key-value/utils/tools.h"
#include "key-value/benchmark.h"


typedef keyvalue::meta meta_type;

int main(int argc, char* argv[]) {

    // build argument from the command line
    argument a(argc, argv);
    std::cout << a << std::endl;
    // build the bench infunction of the arguement
    benchmark<meta_type> b(a);
    //bench
    run_loop(b);
    
	return 0;
}
