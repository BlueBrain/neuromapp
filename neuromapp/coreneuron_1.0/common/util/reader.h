#ifndef _init_
#define _init_

#include <stdio.h>
#include <stdlib.h>
#include "coreneuron_1.0/common/memory/nrnthread.h"

void read_nt_from_file(const char *filename, NrnThread *nt);
void write_nt_to_file(char *filename, NrnThread *nt);
void print_iarray(int * data, int n);
void write_darray_to_file(FILE *hFile, double *data, int n);
void write_iarray_to_file(FILE *hFile, int *data, int n);

/* get the rhs data for post processing */
double* get_rhs(const NrnThread * nt);
/* get the d data for post processing */
double* get_d(const NrnThread * nt);
/* get the end data for post processing */
int get_end(const NrnThread * nt);



#endif // _init_
