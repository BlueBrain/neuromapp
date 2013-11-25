#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/hocdec.h"
#ifdef MEM_PROF_BACKTRACE
#include "mem_counter.h"
#endif
#if HAVE_MALLOC_H 
#include <malloc.h>
#endif
#if HAVE_ALLOC_H
#include <alloc.h>	/* at least for turbo C 2.0 */
#endif

//#if HAVE_POSIX_MEMALIGN
#define HAVE_MEMALIGN 1
//#endif


static int emalloc_error=0;

void hoc_malchk() {
	if (emalloc_error) {
		emalloc_error = 0;
		printf("out of memory\n");
                exit(0);
	}
}


#if LINT
double *
#else
char *
#endif
hoc_Emalloc(n)	/* check return from malloc */
	unsigned n;
{
	char *p;

	p = (char *)malloc(n);
	if (p == 0)
		emalloc_error = 1;
#ifdef MEM_PROF_BACKTRACE
        else
        {
          getMemCounter(0, n);
          getMemTraceInfo(0, n, 4);
        }
#endif
#if LINT
	{static double *p; return p;}
#else
	return p;
#endif
}


#if LINT
double *
#else
char *
#endif
hoc_Ecalloc(n, size)	/* check return from calloc */
	unsigned n, size;
{
	char *p;

	if (n == 0) {
		return (char*)0;
	}
	p = (char *)calloc(n, size);
	if (p == 0)
		emalloc_error = 1;
#ifdef MEM_PROF_BACKTRACE
        else
        {
          getMemCounter(1, n*size);
          getMemTraceInfo(1, n*size, 4);
        }
#endif
#if LINT
	{static double *p; return p;}
#else
	return p;
#endif
}


void* nrn_cacheline_alloc(void** memptr, size_t size) {
#if HAVE_MEMALIGN
	static int memalign_is_working = 1;
	if (memalign_is_working) {
		if (posix_memalign(memptr, 64, size) != 0) {
fprintf(stderr, "posix_memalign not working, falling back to using malloc\n");
			memalign_is_working = 0;
			*memptr = hoc_Emalloc(size); hoc_malchk();
		}
#ifdef MEM_PROF_BACKTRACE
                else
                {
                  getMemCounter(2, size);
                  getMemTraceInfo(2, size, 4);
                }
#endif
	}else
#endif
	  *memptr = hoc_Emalloc(size);
        hoc_malchk();
	return *memptr;
}


void* nrn_cacheline_calloc(void** memptr, size_t nmemb, size_t size) {
	int i, n;
#if HAVE_MEMALIGN
	nrn_cacheline_alloc(memptr, nmemb*size);
	memset(*memptr, 0, nmemb*size);
#else
	*memptr = hoc_Ecalloc(nmemb, size); hoc_malchk();
#endif
	return *memptr;
}


#if LINT
double *
#else
char *
#endif
hoc_Erealloc(ptr, size)	/* check return from realloc */
	char *ptr;
	unsigned size;
{
	char *p;

	if (!ptr) {
		return hoc_Emalloc(size);
	}
	p = (char *)realloc(ptr, size);
	if (p == 0) {
		free(ptr);
		emalloc_error = 1;
	}
#ifdef MEM_PROF_BACKTRACE
        else
        {
          getMemCounter(3, size);
          getMemTraceInfo(3, size, 4);
        }
#endif
#if LINT
	{static double *p; return p;}
#else
	return p;
#endif
}

