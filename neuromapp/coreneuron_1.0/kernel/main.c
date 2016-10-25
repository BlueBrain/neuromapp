/*
 * Neuromapp - main.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/cstep/main.c
 * \brief Implements a miniapp combining  the compute kernel and the Hines solver of coreneuron 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils/storage/storage.h"

#include "coreneuron_1.0/kernel/helper.h"
#include "coreneuron_1.0/kernel/kernel.h"
#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"
#include "utils/error.h"

// Get OMP header if available
#include "utils/omp/compatibility.h"

/** \fn compute_wrapper(NrnThread *nt, struct input_parameters* p)
    \brief Start the computation of kernel following the input parameter
    \param nt the data structure where all the datas are saved
    \param p input parameters where are defined the wanted computation
 */
void compute_wrapper(NrnThread *nt, struct input_parameters* p);

int coreneuron10_kernel_execute(int argc, char *const argv[])
{

    struct input_parameters p;
    int error = MAPP_OK;
    error = kernel_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;

    omp_set_num_threads(p.th);

    NrnThread * nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, free_nrnthread);
    if(nt == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }

    //#pragma omp parallel
    {
        NrnThread * ntlocal = (NrnThread *) clone_nrnthread(nt);
        #pragma omp barrier
        compute_wrapper(ntlocal,&p);
        #pragma omp barrier
        #pragma omp single
        {
            storage_put(p.name,ntlocal,free_nrnthread);
            ntlocal=0;
        }
        if (ntlocal) free_nrnthread(ntlocal);
    }
    return error;
}

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
        const size_t mech_id = 18;
        gettimeofday(&tvBegin, NULL);
        if(strncmp(p->f,"state",5) == 0)
            mech_state_ProbAMPANMDA_EMS(nt, &(nt->ml[mech_id]));
        if(strncmp(p->f,"current",7) == 0)
            mech_current_ProbAMPANMDA_EMS(nt, &(nt->ml[mech_id]));
        gettimeofday(&tvEnd, NULL);
    }
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n CURRENT SOA State Version : %s; %s: %ld [s], %ld [us]",
           p->m, p->f, (long) tvDiff.tv_sec, (long) tvDiff.tv_usec);
}
