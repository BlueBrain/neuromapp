/*
* Neuromapp - data_manager.c, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* paramod.kumbhar@epfl.ch
* All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3.0 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.
*/

/**
 * @file neuromapp/coreneuron_1.0/common/memory/data_manager.c
 * Implements openacc technology for coreneuron1.0
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __OPENACC__
#include<openacc.h>
#endif

#include "coreneuron_1.0/common/memory/nrnthread.h"

#define DEBUG 1

void setup_nrnthreads_on_device(NrnThread *nt) {

#ifdef __OPENACC__
    int i;
    /* pointers for data struct on device, starting with d_ */
    NrnThread *d_nt; //NrnThread on device
    double *d__data;  // nrn_threads->_data on device
    Mechanism *d_ml;

    printf("\n -----------COPYING DATA TO DEVICE --------------- \n");

//    nt->_actual_rhs[0] = 13.4;
//    nt->_actual_rhs[1] = 20.4;

    /* -- copy NrnThread to device -- */
    d_nt = acc_copyin(nt, sizeof(NrnThread));

    /* -- copy _data to device -- */

    /*copy all double data for thread */
    d__data = acc_copyin(nt->_data, nt->_ndata*sizeof(double));

    /*update d_nt._data to point to device copy */
    acc_memcpy_to_device(&(d_nt->_data), &d__data, sizeof(double*));

    /* -- setup rhs, d, a, b, v, node_aread to point to device copy -- */
    double *dptr;

    dptr = d__data + 0*nt->end;
    acc_memcpy_to_device(&(d_nt->_actual_rhs), &(dptr), sizeof(double*));

    dptr = d__data + 1*nt->end;
    acc_memcpy_to_device(&(d_nt->_actual_d), &(dptr), sizeof(double*));

    dptr = d__data + 2*nt->end;
    acc_memcpy_to_device(&(d_nt->_actual_a), &(dptr), sizeof(double*));

    dptr = d__data + 3*nt->end;
    acc_memcpy_to_device(&(d_nt->_actual_b), &(dptr), sizeof(double*));

    dptr = d__data + 4*nt->end;
    acc_memcpy_to_device(&(d_nt->_actual_v), &(dptr), sizeof(double*));

    dptr = d__data + 5*nt->end;
    acc_memcpy_to_device(&(d_nt->_actual_area), &(dptr), sizeof(double*));

    /* -- copy membrane list ml to device -- */

    /*copy ml[] from nt */
    d_ml = acc_copyin(nt->ml, nt->nmech*sizeof(Mechanism));

    /*update d_nt.ml to point to device copy */
    acc_memcpy_to_device(&(d_nt->ml), &d_ml, sizeof(double*));

    /* -- for every ml, setup the data pointers and copy the pdata[] -- */

    size_t offset = 6*nt->end;

    for (i=0; i<nt->nmech; i++) {

        Mechanism *ml = &(nt->ml[i]);
        int *d_nodeindices;
        int *d_pdata;

        dptr = d__data+offset;

        acc_memcpy_to_device(&(d_ml[i].data), &(dptr), sizeof(double*));
        offset += ml->nodecount * ml->szp;

         if (!ml->is_art) {
            d_nodeindices = acc_copyin(ml->nodeindices, sizeof(int)*ml->nodecount);
            acc_memcpy_to_device(&(d_ml[i].nodeindices), &d_nodeindices, sizeof(int*));
         }

         if (ml->szdp) {
            d_pdata = acc_copyin(ml->pdata, sizeof(int)*ml->nodecount*ml->szdp);
            acc_memcpy_to_device(&(d_ml[i].pdata), &d_pdata, sizeof(int*));
         }

         /*
          * note: scalars are copied to device, but need to copyback if necessary as
          * acc_update_self(&(nt->ml[i].szp), sizeof(int));
          */
    }


    /*copy shadow_rhs for thread */
    dptr = acc_copyin(nt->_shadow_rhs, nt->max_nodecount*sizeof(double));
    acc_memcpy_to_device(&(d_nt->_shadow_rhs), &dptr, sizeof(double*));

    /*copy shadow_d for thread */
    dptr = acc_copyin(nt->_shadow_d, nt->max_nodecount*sizeof(double));
    acc_memcpy_to_device(&(d_nt->_shadow_d), &dptr, sizeof(double*));

#if 0
    printf("\n Before Addition : %d %lf", nt->end, nt->dt);
    printf("\n RHS 0, 1 : %lf %lf", nt->_actual_rhs[0], nt->_actual_rhs[1]);
    printf("\n Ncompartment: %d", nt->end);

    #pragma acc parallel loop present(nt[0:1])
    for(i=0; i<nt->end; i++) {
        nt->_actual_rhs[i]  = nt->_actual_rhs[i] + i + 0.1;
    }


    acc_update_self(nt->_actual_rhs, nt->end*sizeof(double));

    printf("\n After Addition : %d %lf", nt->end, nt->dt);
    printf("\n RHS 0, 1 : %lf %lf", nt->_actual_rhs[0], nt->_actual_rhs[1]);


    #pragma acc parallel loop present(nt[0:1])
    for(i=0; i<nt->end; i++) {
        nt->_actual_rhs[i]  = nt->_actual_rhs[i] + i + 0.1;
    }
    //acc_update_device(&(nt->end), sizeof(int));
    //acc_update_device(&(nt->dt), sizeof(double));

    acc_update_self(nt->_actual_rhs, nt->end*sizeof(double));

    printf("\n After Addition : %d %lf", nt->end, nt->dt);
    printf("\n RHS 0, 1 : %lf %lf", nt->_actual_rhs[0], nt->_actual_rhs[1]);

#endif
#endif
}

void update_nrnthreads_on_host(NrnThread *nt)
{
#ifdef __OPENACC__
  int i, j;
  int ne = nt->end;

  acc_update_self(nt->_actual_rhs, ne*sizeof(double));
  acc_update_self(nt->_actual_d, ne*sizeof(double));
  acc_update_self(nt->_actual_a, ne*sizeof(double));
  acc_update_self(nt->_actual_b, ne*sizeof(double));
  acc_update_self(nt->_actual_v, ne*sizeof(double));
  acc_update_self(nt->_actual_area, ne*sizeof(double));

    size_t offset = 6*ne;

    for (i=0; i<nt->nmech; i++) {

        Mechanism *ml = &(nt->ml[i]);

        int n = ml->nodecount * ml->szp;
        acc_update_self(ml->data, n*sizeof(double));

         if (!ml->is_art) {
            acc_update_self(ml->nodeindices, ml->nodecount*sizeof(int));
         }

         if (ml->szdp) {
            acc_update_self(ml->pdata, ml->nodecount*ml->szdp*sizeof(int));
         }
     }
#endif
}

/*
Questions :

    //acc_update_self( (void*)nt, sizeof(NrnThread) ); why updated self? entirely overwrite? http://docs.cray.com/cgi-bin/craydoc.cgi?mode=Show;q=;f=man/xt_prgdirm/82/cat7/openacc.examples.7.html
    //acc_memcpy_from_device(nt->_data, d__data,  nt->_ndata*sizeof(double)); Any difference with copyout vs this?

    acc_memcpy_to_device doesn't accept &(ptr+12) ?

*/
