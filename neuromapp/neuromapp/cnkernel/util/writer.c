#include <stdio.h>

#include "cnkernel/memory/nrnthread.h"

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
