#ifndef _writer_
#define _writer_

#include <stdio.h>

#include "coreneuron_1.0/common/memory/nrnthread.h"

/** \fn int read_nt_from_file(const char *filename, NrnThread *nt)
 \brief read the input data and set up all fields of nrnthread
 \param filename path of the input data
 \param NrnThread Data structure of the memory
 */
void dump_darray_to_file(char *fname, double *d, int n);
void dump_iarray_to_file(char *fname, int *d, int n);
void write_darray_to_file(FILE *hFile, double *data, int n);
void write_iarray_to_file(FILE *hFile, int *data, int n);
void write_nt_to_file(char *filename, NrnThread *nt);

#endif // _writer_
