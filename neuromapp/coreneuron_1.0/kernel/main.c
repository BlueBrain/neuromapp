#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils/storage/storage.h"

#include "coreneuron_1.0/kernel/helper.h"
#include "coreneuron_1.0/kernel/kernel.h"
#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/writer.h"
#include "coreneuron_1.0/common/util/timer.h"
#include "coreneuron_1.0/common/memory/data_manager.h"
#include "utils/error.h"

#ifdef _OPENMP
#include <omp.h>
#endif


void compute(NrnThread *nt, struct input_parameters* p);
void compute_wrapper(NrnThread *nt, struct input_parameters* p);

int coreneuron10_kernel_execute(int argc, char *const argv[])
{

    struct input_parameters p;
    int error = MAPP_OK;
    error = kernel_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;

#ifdef _OPENMP
    omp_set_num_threads(p.th); // set up the number of thread
#endif

    printf("\n Starting Initialization!");
    fflush(stdout);

    NrnThread * nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, dealloc_nrnthread);
    if(nt == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }

    #pragma omp parallel
    {
        NrnThread * ntlocal = clone_nrnthread(nt);
        #pragma omp barrier
        compute(ntlocal,&p);
        #pragma omp barrier
        #pragma omp single
        {
            storage_put(p.name,ntlocal,dealloc_nrnthread); //enjoy debug
            ntlocal=0;
        }
        if (ntlocal) dealloc_nrnthread(ntlocal);
    }
    return error;
}

void compute(NrnThread *nt, struct input_parameters *p)
{
    setup_nrnthreads_on_device(nt) ;
    compute_wrapper(nt,p);
}

///
// \brief compute
// \param nt (In) Data structure for a neuron cell group containing all information
// \param mech_id (In) id of the mechanism as ordered in bbpcore file. A translation table is required from mechanism
// \                   id to bbpcoreid
void compute_wrapper(NrnThread *nt, struct input_parameters *p)
{
    if(strncmp(p->m,"Na",2) == 0)
    {
        const size_t mech_id = 17;
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
             mech_state_NaTs2_t(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
             mech_current_NaTs2_t(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }

    if(strncmp(p->m,"Ih",2) == 0)
    {
        const size_t mech_id = 10;
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
             mech_current_Ih(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
             mech_state_Ih(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }

    if(strncmp(p->m,"ProbAMPANMDA",12) == 0)
    {
        const size_t mech_id = 18; //ProbAMPANMDA_EMS
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
            mech_state_ProbAMPANMDA_EMS(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
            mech_current_ProbAMPANMDA_EMS(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n CURRENT SOA State Version : %ld [s] %ld [us]", (long) tvDiff.tv_sec, (long) tvDiff.tv_usec);
}