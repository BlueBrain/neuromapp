#ifndef _init_
#define _init_

#include <stdio.h>
#include "cnkernel/memory/nrnthread.h"

void read_nt_from_file(char *filename, NrnThread *nt);
void write_nt_to_file(char *filename, NrnThread *nt);
void print_iarray(int * data, int n);
void write_darray_to_file(FILE *hFile, double *data, int n);
void write_iarray_to_file(FILE *hFile, int *data, int n);

#endif // _init_
