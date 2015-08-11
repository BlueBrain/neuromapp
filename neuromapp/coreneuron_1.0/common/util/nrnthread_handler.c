/*
* Neuromapp - nrnthread_handler.c, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
* Sam Yates - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* paramod.kumbhar@epfl.ch
* sam.yates@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/common/util/nrnthread_handler.c
 * Implements function for the allocation, initialisation and copy of the memory for coreneuron1.0
 */


#include <string.h>
#include <stdlib.h>

#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "utils/error.h"

void * make_nrnthread(void * context) {
    const char * filename = (const char*)context;
    void * nt = malloc(sizeof(NrnThread));
    int error = read_nt_from_file(filename, (NrnThread*)nt);
    if(error != MAPP_OK){ // looking for not relevant data
        free(nt); // no memory leaks
        return NULL;
    }
    return nt;
}

void dealloc_nrnthread(void * p) {
    if(!p)
        return; // the previous allocation failed

    NrnThread * nt = (NrnThread * )p;

    free(nt->_shadow_d);
    nt->_shadow_d = NULL;
    free(nt->_shadow_rhs);
    nt->_shadow_rhs = NULL;
    free(nt->_v_parent_index);
    nt->_v_parent_index = NULL;

    int i;
    for (i=nt->nmech-1; i>=0; i--) {
        Mechanism *ml = &nt->ml[i];
        free(ml->pdata);
        ml->pdata = NULL;
        free(ml->nodeindices);
        ml->nodeindices = NULL;
    }

    free(nt->ml);
    nt->ml = NULL;
    free(nt->_data);
    nt->_data = NULL;
    free(nt);
    nt = NULL;
}

static void *memcpy_align(void *s, size_t align, size_t size) {
    void *d;
    posix_memalign(&d, align, size);
    memcpy(d, s, size);
    return d;
}

NrnThread* clone_nrnthread(NrnThread const* p){
    int i;
    long int offset;
    int ne;

    NrnThread *nt=(NrnThread *)malloc(sizeof(NrnThread));
    nt->dt = p->dt;
    nt->_ndata = p->_ndata;

    nt->_data = memcpy_align(p->_data, 64, sizeof(double) * nt->_ndata);

    nt->end = p->end;
    ne = nt->end;

    nt->_actual_rhs = nt->_data + 0*ne;
    nt->_actual_d = nt->_data + 1*ne;
    nt->_actual_a = nt->_data + 2*ne;
    nt->_actual_b = nt->_data + 3*ne;
    nt->_actual_v = nt->_data + 4*ne;
    nt->_actual_area = nt->_data + 5*ne;

    offset = 6*ne;
    nt->nmech = p->nmech;

    nt->ml = (Mechanism *) calloc(sizeof(Mechanism), nt->nmech);
    nt->max_nodecount = 0;

    for (i=0; i<nt->nmech; i++) {

        Mechanism *ml = &nt->ml[i];
        Mechanism const *pml = &p->ml[i];
        ml->type = pml->type;
        ml->is_art = pml->is_art;
        ml->nodecount = pml->nodecount;
        ml->szp = pml->szp;
        ml->szdp = pml->szdp;
        ml->offset = pml->offset;
        ml->data = nt->_data + offset;
        offset += ml->nodecount * ml->szp;

        if ( nt->max_nodecount < ml->nodecount)
            nt->max_nodecount = ml->nodecount;

        if (!ml->is_art)
            ml->nodeindices = memcpy_align(pml->nodeindices, 64, sizeof(int) * ml->nodecount);

        if (ml->szdp)
            ml->pdata=memcpy_align(pml->pdata, 64, sizeof(int) * ml->nodecount*ml->szdp);

    }

    /* parent indexes for linear algebra */
    nt->_v_parent_index=memcpy_align(p->_v_parent_index, 64, sizeof(int) * ne);

    /* no of cells in the dataset */
    nt->ncell = p->ncell;

    posix_memalign((void **)&nt->_shadow_rhs, 64, sizeof(double) *nt->max_nodecount);
    posix_memalign((void **)&nt->_shadow_d, 64, sizeof(double) *nt->max_nodecount);

    return nt;
}

