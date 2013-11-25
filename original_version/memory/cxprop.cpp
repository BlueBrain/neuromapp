/*
allocate and free property data and Datum arrays for nrniv
this allows for the possibility of
greater cache efficiency
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "include/membfunc.h"
#include "include/hocdec.h"
#include "include/arraypool.h"
#include "include/section.h"

void* nrn_cacheline_calloc(void** memptr, size_t nmemb, size_t size);

declareArrayPool(CharArrayPool, char)
implementArrayPool(CharArrayPool, char)

//#define APSIZE 700001
#define APSIZE 100000 // 1000 channel of type Na
declareArrayPool(DoubleArrayPool, double)
implementArrayPool(DoubleArrayPool, double)
declareArrayPool(DatumArrayPool, Datum)
implementArrayPool(DatumArrayPool, Datum)

static int force;
static int npools_;
static DoubleArrayPool** dblpools_;
static DatumArrayPool** datumpools_;
static void mk_prop_pools(int n);


/*
Based on the nrn_threads tml->ml->nodelist order from the last
call to nrn_cache_prop_realloc() (which writes a data file) from
a previous launch, on this launch, read that file and create
pools matched to the space needed by each thread and which, from
the sequence of data allocation requests returns space that ends
up being laid out in memory in just the way we want.
The data file format is
maxtype // so we can be sure we have large enough npools_
nthread // number of threads
nmech // number of mechanisms used in this thread
type sz1 sz2 ntget cnt // mechanism type, double/Datum array size total number of times alloc was called, how many needed for this thread
// the above specifies the pool allocation
// note that the pool chain order is the same as the thread order
// there are nthread of the following lists
cnt // number of mechanisms in thread
type i seq // i is the tml->ml->data[i], seq is the allocation order
// ie we want

Note that the overall memory allocation sequence has to be identical
to the original sequence in terms of get/put for the final
layout to be exactly right for cache efficiency and for threads not
to share cache lines. However, if this is not the case, the memory allocation
is still correct, just not as efficient.
*/

static void mk_prop_pools(int n) {
	int  i;
	if (n > npools_) {
		DoubleArrayPool** p1 = new DoubleArrayPool*[n];
		DatumArrayPool** p2 = new DatumArrayPool*[n];
		for (i=0; i < n; ++i) {
			p1[i] = 0;
			p2[i] = 0;
		}
		if (dblpools_) {
			for (i=0; i < npools_; ++i) {
				p1[i] = dblpools_[i];
				p2[i] = datumpools_[i];
			}
			delete [] dblpools_;
			delete [] datumpools_;
		}
		dblpools_ = p1;
		datumpools_ = p2;
		npools_ = n;
	}
}

void nrn_mk_prop_pools(int n) {
	mk_prop_pools(n);
}

double* nrn_prop_data_alloc(int type, int count, Prop* p) {
	if (!dblpools_[type]) {
		dblpools_[type] = new DoubleArrayPool(APSIZE, count);
	}
	assert(dblpools_[type]->d2() == count);
	p->_alloc_seq = dblpools_[type]->ntget();
	double* pd = dblpools_[type]->alloc();
	return pd;
}

Datum* nrn_prop_datum_alloc(int type, int count, Prop* p) {
	int i;
	Datum* ppd;
	if (!datumpools_[type]) {
		datumpools_[type] = new DatumArrayPool(APSIZE, count);
	}
	assert(datumpools_[type]->d2() == count);
	p->_alloc_seq = datumpools_[type]->ntget();
	ppd = datumpools_[type]->alloc();
	for (i=0; i < count; ++i) { ppd[i]._pvoid = 0; }
	return ppd;
}

void nrn_prop_data_free(int type, double* pd) {
	if (pd) {
		dblpools_[type]->hpfree(pd);
	}
}

void nrn_prop_datum_free(int type, Datum* ppd) {
	if (ppd) {
		datumpools_[type]->hpfree(ppd);
	}
}

void single_prop_free(Prop* p)
{
//        extern char* pnt_map;
//        if (pnt_map[p->type]) {
//                clear_point_process_struct(p);
//                return;
//        }
        if (p->param) {
//                notify_freed_val_array(p->param, p->param_size);
                nrn_prop_data_free(p->type, p->param);
        }
        if (p->dparam) {
//                if (p->type == CABLESECTION) {
//                        notify_freed_val_array(&p->dparam[2].val, 6);
//                }
                nrn_prop_datum_free(p->type, p->dparam);
        }
//        if (p->ob) {
//                hoc_obj_unref(p->ob);
//        }
        free((char *)p);
}


void prop_free(Prop **pp)
{
        Prop *p, *pn;
        p = *pp;
        *pp = (Prop *)0;
        while (p) {
                pn = p->next;
                single_prop_free(p);
                p = pn;
        }
}


