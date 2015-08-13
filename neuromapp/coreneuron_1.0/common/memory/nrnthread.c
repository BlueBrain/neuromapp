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

#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "utils/error.h"

int nrnthread_dealloc(NrnThread *nt) {
    int i;

    free(nt->_shadow_d);
    nt->_shadow_d = NULL;

    free(nt->_shadow_rhs);
    nt->_shadow_rhs = NULL;

    free(nt->_v_parent_index);
    nt->_v_parent_index = NULL;

    for (i=nt->nmech-1; i>=0; --i) {
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

    return MAPP_OK;
}

static void *memcpy_align(void *s, size_t align, size_t size) {
    void *d;
    posix_memalign(&d, align, size);
    memcpy(d, s, size);
    return d;
}

int nrnthread_copy(const NrnThread *p, NrnThread *nt){
    int i;
    long int offset;
    int ne;

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

    return MAPP_OK;
}

/** /brief Scan and discard up to and including next newline. */
static void skip_line(FILE *hFile) {
    int c;
    do {
        c = fgetc(hFile);
    } while (c!=EOF && c!='\n');
}

/** /brief Read NrnThread double vector */
static void read_nrnthread_darray(FILE *hFile, double *data, int n) {
    int i;
    for(i=0; i<n; i++) {
        fscanf(hFile, "%lf\n", &data[i]);
    }
    skip_line(hFile);
}

/** /brief Read NrnThread int vector */
static void read_nrnthread_iarray(FILE *hFile, int *data, int n) {
    int i;
    for(i=0; i<n; i++) {
        fscanf(hFile, "%d\n", &data[i]);
    }
    skip_line(hFile);
}

/** /brief Write NrnThread double vector */
static void write_nrnthread_darray(FILE *hFile, const double *data, int n) {
    int i;
    for(i=0; i<n; i++) {
        fprintf(hFile, "%lf\n", data[i]);
    }
    fputs("---\n",hFile);
}

/** /brief Write NrnThread int vector */
static void write_nrnthread_iarray(FILE *hFile, const int *data, int n) {
    int i;
    for(i=0; i<n; i++) {
        fprintf(hFile, "%d\n", data[i]);
    }
    fputs("---\n",hFile);
}

int nrnthread_read(FILE *hFile, NrnThread *nt) {
    int i;
    long int offset;
    int ne;

    if (!hFile)
        return MAPP_BAD_DATA; // the input does not exists stop;

    nt->dt = 0.025;

    fscanf(hFile, "%d\n", &nt->_ndata);
    posix_memalign( (void **) &nt->_data, 64, sizeof(double) * nt->_ndata);
    read_nrnthread_darray(hFile, nt->_data, nt->_ndata);

    fscanf(hFile, "%d\n", &nt->end);
    ne = nt->end;

    nt->_actual_rhs = nt->_data + 0*ne;
    nt->_actual_d = nt->_data + 1*ne;
    nt->_actual_a = nt->_data + 2*ne;
    nt->_actual_b = nt->_data + 3*ne;
    nt->_actual_v = nt->_data + 4*ne;
    nt->_actual_area = nt->_data + 5*ne;

    offset = 6*ne;
    fscanf(hFile, "%d\n", &nt->nmech);

    nt->ml = (Mechanism *) calloc(sizeof(Mechanism), nt->nmech);

    nt->max_nodecount = 0;

    for (i=0; i<nt->nmech; i++) {

        Mechanism *ml = &nt->ml[i];
        fscanf(hFile, "%d %d %d %d %d %ld\n", &(ml->type), &(ml->is_art),
               &(ml->nodecount), &(ml->szp), &(ml->szdp), &(ml->offset));
        ml->data = nt->_data + offset;
        offset += ml->nodecount * ml->szp;

        if ( nt->max_nodecount < ml->nodecount)
            nt->max_nodecount = ml->nodecount;

        printf("=> Mechanism type %d is at index %d\n", ml->type, i);

        if (!ml->is_art) {
            posix_memalign((void **)&ml->nodeindices, 64, sizeof(int) * ml->nodecount);
            read_nrnthread_iarray(hFile, ml->nodeindices, ml->nodecount);
        }

        if (ml->szdp) {
            posix_memalign((void **)&ml->pdata, 64, sizeof(int) * ml->nodecount*ml->szdp);
            read_nrnthread_iarray(hFile, ml->pdata, ml->nodecount*ml->szdp);
        }
    }

    /* parent indexes for linear algebra */
    posix_memalign((void **)&nt->_v_parent_index, 64, sizeof(int) * ne);
    read_nrnthread_iarray(hFile, nt->_v_parent_index, ne);

    /* no of cells in the dataset */
    fscanf(hFile, "%d\n", &nt->ncell);

    posix_memalign((void **)&nt->_shadow_rhs, 64, sizeof(double) *nt->max_nodecount);
    posix_memalign((void **)&nt->_shadow_d, 64, sizeof(double) *nt->max_nodecount);

    return MAPP_OK;
}

int nrnthread_write(FILE *hFile, const NrnThread *nt) {
    int i;
    long int offset;
    int ne;

    fprintf(hFile, "%d\n", nt->_ndata);
    write_nrnthread_darray(hFile, nt->_data, nt->_ndata);

    fprintf(hFile, "%d\n", nt->end);
    ne = nt->end;

    offset = 6*ne;
    fprintf(hFile, "%d\n", nt->nmech);

    for (i=0; i<nt->nmech; i++) {
        Mechanism *ml = &nt->ml[i];
        fprintf(hFile, "%d %d %d %d %d %ld\n", ml->type, ml->is_art, ml->nodecount, ml->szp, ml->szdp, ml->offset);
        ml->data = nt->_data + offset;
        offset += ml->nodecount * ml->szp;

        if (!ml->is_art) {
            write_nrnthread_iarray(hFile, ml->nodeindices, ml->nodecount);
        }

        if (ml->szdp) {
            write_nrnthread_iarray(hFile, ml->pdata, ml->nodecount*ml->szdp);
        }
    }

  fclose(hFile);
  return MAPP_OK;
}


