#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "coreneuron_1.0/solver/helper.h"
#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/solver/solver.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/reader.h"
#include "coreneuron_1.0/common/util/timer.h"

int coreneuron10_solver_execute(int argc, char * argv[])
{

    struct input_parameters p;
    solver_help(argc, argv, &p);

     if(argc < 2)
    {
        printf("\n Error! Provide directory path of data files! \n");
        exit(1);
    }
    else
    {
        NrnThread nt;
        read_nt_from_file(p.d, &nt);

        gettimeofday(&tvBegin, NULL);

        nrn_solve_minimal(&nt);

        gettimeofday(&tvEnd, NULL);

        timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
        printf("\n Time For Hines Solver : %ld.%06d [s] %ld.%06ld [us]", tvDiff.tv_sec, tvDiff.tv_usec);

    }
}
