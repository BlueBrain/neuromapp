#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "utils/storage/storage.h"

#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/solver/solver.h"

#include "coreneuron_1.0/kernel/kernel.h"
#include "coreneuron_1.0/kernel/mechanism/mechanism.h"

#include "coreneuron_1.0/cstep/helper.h"
#include "coreneuron_1.0/cstep/cstep.h"

#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"

int getMechId(char * mechName);
Mechanism* getMechFromString(NrnThread * nt, char * mechName);
int coreneuron10_cstep_execute(int argc, char * const argv[]);

int coreneuron10_cstep_execute(int argc, char * const argv[]) {
    struct input_parameters p;
    cstep_help(argc, argv, &p);

    if (argc < 2) {
        printf("\n Error! Provide directory path of data files! \n");
        return 1;
    }

    //Gets the data
    NrnThread * nt = (NrnThread *) storage_get(p.name, make_nrnthread, p.d, dealloc_nrnthread);

    //Initial mechanisms set-up already done in the input date (no need to call mech_init_Ih, etc)
    gettimeofday(&tvBegin, NULL);

    //Load mechanisms
    mech_current_NaTs2_t(nt,&(nt->ml[17]));
    mech_current_Ih(nt,&(nt->ml[10]));
    mech_current_ProbAMPANMDA_EMS(nt,&(nt->ml[18]));

    //Call solver
    nrn_solve_minimal(nt);

    //Update the states
    mech_state_NaTs2_t(nt,&(nt->ml[17]));
    mech_state_Ih(nt,&(nt->ml[10]));
    mech_state_ProbAMPANMDA_EMS(nt,&(nt->ml[18]));

    gettimeofday(&tvEnd, NULL);
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    
    printf("\nTime for full computational step: %ld [s] %ld [us]\n", tvDiff.tv_sec, (long) tvDiff.tv_usec);
    return 0;
}
