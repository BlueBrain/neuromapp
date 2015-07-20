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

#ifdef _OPENMP
#include <omp.h>
#endif


void compute(NrnThread *nt,size_t mech_id, struct input_parameters* p);
void compute_wrapper(NrnThread *nt, size_t mech_i, struct input_parameters* p);

int coreneuron10_kernel_execute(int argc, char *const argv[])
{

    struct input_parameters p;
    kernel_help(argc, argv, &p);

#ifdef _OPENMP
    omp_set_num_threads(p.th); // set up the number of thread
#endif

    if(argc < 2)
    {
        printf("\n Error! Provide directory path of data files! \n");
        exit(1);
    }
    else
    {
        printf("\n Starting Initialization!");
        fflush(stdout);

        #pragma omp parallel
        {
            NrnThread * nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, dealloc_nrnthread);

            printf("\n Finished Initialization!");
            fflush(stdout);

            int mech_id = 17; // atoi(argc[2]);
            compute(nt, mech_id,&p);
        }
    }
    return 0;
}

void compute(NrnThread *nt, size_t mech_id, struct input_parameters *p)
{
    setup_nrnthreads_on_device(nt) ;
    compute_wrapper(nt, mech_id,p);
}

///
// \brief compute
// \param nt (In) Data structure for a neuron cell group containing all information
// \param mech_id (In) id of the mechanism as ordered in bbpcore file. A translation table is required from mechanism
// \                   id to bbpcoreid
void compute_wrapper(NrnThread *nt, size_t mech_id, struct input_parameters *p)
{

    if(strncmp(p->m,"Na",2) == 0)
    {
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
             mech_state_NaTs2_t(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
             mech_current_NaTs2_t(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }

    if(strncmp(p->m,"Ih",2) == 0)
    {
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
             mech_state_NaTs2_t(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
             mech_current_NaTs2_t(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }

    if(strncmp(p->m,"ProbAMPANMDA",12) == 0)
    {
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
            mech_state_ProbAMPANMDA_EMS(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
            mech_current_ProbAMPANMDA_EMS(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }

    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n CURRENT SOA State Version : %ld [s] %ld [us]", tvDiff.tv_sec, tvDiff.tv_usec);
}
