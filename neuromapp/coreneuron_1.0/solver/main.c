#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils/storage/storage.h"

#include "coreneuron_1.0/solver/helper.h"
#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/solver/solver.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"
#include "utils/error.h"

int coreneuron10_solver_execute(int argc, char * const argv[])
{

    struct input_parameters p;
    int error = MAPP_OK; //so far, so good
    error = solver_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;

    NrnThread * nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, dealloc_nrnthread);
    if(nt == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }
    gettimeofday(&tvBegin, NULL);
    nrn_solve_minimal(nt);
    gettimeofday(&tvEnd, NULL);

    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n Time For Hines Solver : %ld [s] %ld [us]", tvDiff.tv_sec, (long) tvDiff.tv_usec);

    return error;
}
