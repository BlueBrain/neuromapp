/*
 * Neuromapp - writer.c, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/common/util/writer.c
 * Implements function for the input data reading
 */

#include <stdio.h>

#include "coreneuron_1.0/common/memory/nrnthread.h"

void write_darray_to_file(FILE *hFile, double *data, int n) {
    int i;
    char buf[4096]="---\n";

    for(i=0; i<n; i++) {
        fprintf(hFile, "%lf\n", data[i]);
    }
    fprintf(hFile, "%s", buf);
}

void write_iarray_to_file(FILE *hFile, int *data, int n) {
    int i;
    char buf[4096]="---\n";

    for(i=0; i<n; i++) {
        fprintf(hFile, "%d\n", data[i]);
    }
    fprintf(hFile, "%s", buf);
}

void dump_darray_to_file(char *fname, double *d, int n)
{
    FILE *fp = fopen(fname, "w");
    write_darray_to_file(fp, d, n);
    fclose(fp);
}

void dump_iarray_to_file(char *fname, int *d, int n)
{
    FILE *fp = fopen(fname, "w");
    write_iarray_to_file(fp, d, n);
    fclose(fp);
}

void write_nt_to_file(char *filename, NrnThread *nt)
{
    int i;
    long int offset;
    FILE *hFile;
    int ne;

    hFile = fopen(filename, "w");

    fprintf(hFile, "%d\n", nt->_ndata);
    write_darray_to_file(hFile, nt->_data, nt->_ndata);

    fprintf(hFile, "%d\n", nt->end);
    ne = nt->end;

    nt->_actual_rhs = nt->_data + 0*ne;
    nt->_actual_d = nt->_data + 1*ne;
    nt->_actual_a = nt->_data + 2*ne;
    nt->_actual_b = nt->_data + 3*ne;
    nt->_actual_v = nt->_data + 4*ne;
    nt->_actual_area = nt->_data + 5*ne;

    offset = 6*ne;
    fprintf(hFile, "%d\n", nt->nmech);

    for (i=0; i<nt->nmech; i++) {

        Mechanism *ml = &nt->ml[i];
        fprintf(hFile, "%d %d %d %d %d %ld\n", ml->type, ml->is_art, ml->nodecount, ml->szp, ml->szdp, ml->offset);
        ml->data = nt->_data + offset;
        offset += ml->nodecount * ml->szp;

        //printf("\n Mech Id %d with Index %d : %d %d %d %d %d %ld" , ml->type, i, (ml->type), (ml->is_art), (ml->nodecount), (ml->szp), (ml->szdp), (ml->offset));

        if (!ml->is_art) {
            write_iarray_to_file(hFile, ml->nodeindices, ml->nodecount);
        }

        if (ml->szdp) {
            write_iarray_to_file(hFile, ml->pdata, ml->nodecount*ml->szdp);
        }
    }

  fclose(hFile);
}
