// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "key-value/utils/tools.h"
#include "key-value/benchmark.h"
#include "key-value/utils/statistics.h"



int main(int argc, char* argv[]) {


    // build argument from the command line
    argument a(argc, argv);
    // build the bench infunction of the argument
    if(a.backend() == "map"){
        typedef keyvalue::meta meta_type;
        benchmark<meta_type> b(a);
        //bench
        statistic s = run_loop(b);
        //compute statistics
        s.process();
        //print the results
        std::cout << s << std::endl;
    }


	return 0;
}
