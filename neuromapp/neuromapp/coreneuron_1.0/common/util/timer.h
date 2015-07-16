#pragma once

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

struct timeval tvBegin, tvEnd, tvDiff;

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);

