#ifndef nrnthread_handler_h
#define nrnthread_handler_h

#include <stdlib.h>
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/reader.h"

void * make_nrnthread(void * filename);
void dealloc_nrnthread(void * p);
NrnThread* clone_nrnthread(NrnThread const* p);

#endif
