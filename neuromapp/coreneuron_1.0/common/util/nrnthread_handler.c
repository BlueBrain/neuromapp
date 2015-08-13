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

#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"

void *make_nrnthread(void *filename) {
    int r;
    FILE *fh = fopen((const char *)filename, "r");
    if (!fh) return NULL;

    NrnThread *nt = malloc(sizeof(NrnThread));
    r = nrnthread_read(fh, nt);
    fclose(fh);

    if (r) { /* error in read */
        free_nrnthread(nt);
        return NULL;
    }

    return (void *)nt;
}

void *clone_nrnthread(void *p) {
    int r;
    if (!p) return NULL;

    NrnThread *nt = malloc(sizeof(NrnThread));
    r = nrnthread_copy((NrnThread *)p, nt);

    if (r) { /* error in read */
        free_nrnthread(nt);
        return NULL;
    }

    return (void *)nt;
}

void free_nrnthread(void *p) {
    nrnthread_dealloc((NrnThread *)p);
    free(p);
}



