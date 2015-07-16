#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <mpi.h>
#include "coreneuron_1.0/kernel/helper.h"
#include "coreneuron_1.0/kernel/kernel.h"
#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/util/reader.h"
#include "coreneuron_1.0/common/util/writer.h"
#include "coreneuron_1.0/common/util/timer.h"
#include "coreneuron_1.0/common/memory/data_manager.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef __OPENACC__
#include<openacc.h>
#endif

#ifdef CRAYPAT
#include <pat_api.h>
#endif

void compute(NrnThread *nt,size_t mech_id, struct input_parameters* p);
void compute_wrapper(NrnThread *nt, size_t mech_i, struct input_parameters* p);
int gettable()
{
/*
 => Mechanism type 3 is at index 0
 => Mechanism type 4 is at index 1
 => Mechanism type 15 is at index 2
 => Mechanism type 16 is at index 3
 => Mechanism type 28 is at index 4
 => Mechanism type 32 is at index 5
 => Mechanism strncmptype 7 is at index 6
 => Mechanism type 35 is at index 7
 => Mechanism type 44 is at index 8
 => Mechanism type 45 is at index 9
 => Mechanism type 69 is at index 10
 => Mechanism type 71 is at index 11
 => Mechanism type 82 is at index 12
 => Mechanism type 91 is at index 13
 => Mechanism type 98 is at index 14
 => Mechanism type 119 is at index 15
 => Mechanism type 122 is at index 16
 => Mechanism type 125 is at index 17
 => Mechanism type 134 is at index 18
 => Mechanism type 136 is at index 19
 => Mechanism type 141 is at index 20
 => Mechanism type 144 is at index 21
 => Mechanism type 26 is at index 22
 => Mechanism type 129 is at index 23 */
  return 0;
}

int coreneuron10_kernel_execute(int argc, char *argv[])
{

    struct input_parameters p;
    kernel_help(argc, argv, &p);


#ifdef _OPENMP
    omp_set_num_threads(p.th); // set up the number of thread
#endif
    //MPI_Init(NULL, NULL);

#ifdef CRAYPAT
    PAT_record(PAT_STATE_OFF);
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
            NrnThread nt;
            read_nt_from_file(p.d, &nt);

            printf("\n Finished Initialization!");
            fflush(stdout);

#ifdef CRAYPAT
        PAT_record(PAT_STATE_ON);
        PAT_region_begin (1, "STATE");
#endif
            int mech_id = 17; // atoi(argc[2]);
            compute(&nt, mech_id,&p);
        }

#ifdef CRAYPAT
        PAT_region_end(1);
        PAT_record(PAT_STATE_OFF);
#endif

    }

#ifdef __OPENACC__
    acc_shutdown ( acc_device_default);
#endif
    printf("\n Finished Programme\n");

    //MPI_Finalize();
    return 0;
}

void compute(NrnThread *nt, size_t mech_id, struct input_parameters *p)
{
    //printf("The mechanism id from bbpcore: %id"
    setup_nrnthreads_on_device(nt) ;
    compute_wrapper(nt, mech_id,p);
//    update_nrnthreads_on_host(nt);
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
    printf("\n CURRENT SOA State Version : %ld.%06d [s] %ld.%06ld [us]", tvDiff.tv_sec, tvDiff.tv_usec);
}
