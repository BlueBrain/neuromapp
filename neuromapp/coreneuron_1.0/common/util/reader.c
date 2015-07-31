#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coreneuron_1.0/common/memory/nrnthread.h"

void print_iarray(int * data, int n) {
    int i = 0;
    printf ("\n N = %d \n", n);

    for(i = 0; i< n ; i++)
        printf("\t %d", data[i]);
    printf("\n");
}

void read_darray_from_file(FILE *hFile, double *data, int n) {
    int i;
    char buf[4096];

    for(i=0; i<n; i++) {
        fscanf(hFile, "%lf\n", &data[i]);
    }
    fscanf(hFile, "%s", buf);
    //printf("\n%s", buf);
}

void read_iarray_from_file(FILE *hFile, int *data, int n) {
    int i;
    char buf[4096];

    for(i=0; i<n; i++) {
        fscanf(hFile, "%d\n", &data[i]);
    }
    fscanf(hFile, "%s", buf);
    //printf("\n%s", buf);
}

void read_nt_from_file(const char *filename, NrnThread *nt) {

    int i;
    long int offset;
    FILE *hFile;
    int ne;

    hFile = fopen(filename, "r");

    nt->dt = 0.025;

    fscanf(hFile, "%d\n", &nt->_ndata);
    posix_memalign( (void **) &nt->_data, 64, sizeof(double) * nt->_ndata);
    read_darray_from_file(hFile, nt->_data, nt->_ndata);

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
            read_iarray_from_file(hFile, ml->nodeindices, ml->nodecount);
        }

        if (ml->szdp) {
            posix_memalign((void **)&ml->pdata, 64, sizeof(int) * ml->nodecount*ml->szdp);
            read_iarray_from_file(hFile, ml->pdata, ml->nodecount*ml->szdp);
        }
    }

    /* parent indexes for linear algebra */
    posix_memalign((void **)&nt->_v_parent_index, 64, sizeof(int) * ne);
    read_iarray_from_file(hFile, nt->_v_parent_index, ne);

    /* no of cells in the dataset */
    fscanf(hFile, "%d\n", &nt->ncell);

    posix_memalign((void **)&nt->_shadow_rhs, 64, sizeof(double) *nt->max_nodecount);
    posix_memalign((void **)&nt->_shadow_d, 64, sizeof(double) *nt->max_nodecount);

    fclose(hFile);
}
