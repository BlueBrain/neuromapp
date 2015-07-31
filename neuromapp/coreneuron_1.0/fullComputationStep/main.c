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

#include "coreneuron_1.0/fullComputationStep/helper.h"
#include "coreneuron_1.0/fullComputationStep/fullComputationStep.h"

#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"

int getMechId(char * mechName);
Mechanism* getMechFromString(NrnThread * nt, char * mechName);
int coreneuron10_fullComputationStep_execute(int argc, char * const argv[]);

/**
 * Returns the mech ID (internal to NEURON) of a given mechanism
 * @param mechName mechanism name
 * @return mech Id (-1 if success)
 */
int getMechId(char * mechName) {

    if (!strncmp(mechName, "Na", 2)) return 17;
    if (!strncmp(mechName, "Ih", 2)) return 10;
    if (!strncmp(mechName, "ProbAMPANDMA", 12)) return 18;

    printf("\nERROR: No mech ID for mechanism %s. Bye...\n", mechName);
    assert(0);
    return -1;
}

/**
 * Given a NrnThread and a Mechanism name, return a pointer to a mechanism
 * @param nt NrNThread
 * @param mechName Mechanist identifying name
 * @return 
 */
Mechanism* getMechFromString(NrnThread * nt, char * mechName) {
    return &(nt->ml[getMechId(mechName)]);
}

int coreneuron10_fullComputationStep_execute(int argc, char * const argv[]) {
    struct input_parameters p;
    fullComputationStep_help(argc, argv, &p);

    if (argc < 2) {
        printf("\n Error! Provide directory path of data files! \n");
        exit(1);
    }

    //Gets the data
    NrnThread * nt = (NrnThread *) storage_get(p.name, make_nrnthread, p.d, dealloc_nrnthread);

    //Initial mechanisms set-up already done in the input date (no need to call mech_init_Ih, etc)

    gettimeofday(&tvBegin, NULL);

    //Load mechanisms
    mech_current_NaTs2_t(nt, getMechFromString(nt, "Na"));
    mech_current_Ih(nt, getMechFromString(nt, "Ih"));
    mech_current_ProbAMPANMDA_EMS(nt, getMechFromString(nt, "ProbAMPANDMA"));

    //Call solver
    nrn_solve_minimal(nt);

    //Update the states
    mech_state_NaTs2_t(nt, getMechFromString(nt, "Na"));
    mech_state_Ih(nt, getMechFromString(nt, "Ih"));
    mech_state_ProbAMPANMDA_EMS(nt, getMechFromString(nt, "ProbAMPANDMA"));

    gettimeofday(&tvEnd, NULL);
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    
    printf("\nTime for full computational step: %ld [s] %ld [us]\n", tvDiff.tv_sec, (long) tvDiff.tv_usec);
    return 0;
}
