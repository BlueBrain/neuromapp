/*
 * Neuromapp - main.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/solver/main.c
 * Miniapp about the Hines Solver
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils/storage/storage.h"
#include "utils/error.h"

#include "coreneuron_1.0/solver/helper.h"
#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/solver/solver.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"

int coreneuron10_solver_execute(int argc, char * const argv[])
{
    struct input_parameters p;
    int error = MAPP_OK; //so far, so good
    error = solver_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;

    NrnThread * nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, free_nrnthread);

    if(nt == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }
    gettimeofday(&tvBegin, NULL);
    nrn_solve_minimal(nt);
    gettimeofday(&tvEnd, NULL);

    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n Time For Hines Solver : %ld [s] %ld [us]", tvDiff.tv_sec, (long) tvDiff.tv_usec);

    return error;
}
