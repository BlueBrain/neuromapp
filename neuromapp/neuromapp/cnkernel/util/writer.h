#ifndef _writer_
#define _writer_

#include <stdio.h>

#include "cnkernel/memory/nrnthread.h"

void dump_darray_to_file(char *fname, double *d, int n);
void dump_iarray_to_file(char *fname, int *d, int n);
void write_darray_to_file(FILE *hFile, double *data, int n);
void write_iarray_to_file(FILE *hFile, int *data, int n);
void write_nt_to_file(char *filename, NrnThread *nt);

#endif // _writer_
