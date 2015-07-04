#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

struct timeval tvBegin, tvEnd, tvDiff;

/* Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

int test_main()
{

    // begin
    gettimeofday(&tvBegin, NULL);

    // compute here

    //end
    gettimeofday(&tvEnd, NULL);

    // diff
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);

    return 0;
}
