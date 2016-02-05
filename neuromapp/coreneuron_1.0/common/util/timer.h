/* Neuromapp - timer.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/common/util/timer.h
 * \brief provide function for time measuremnt
 */

#ifndef MAPP_TIMER_
#define MAPP_TIMER_

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

struct timeval tvBegin, tvEnd, tvDiff;

/** \fn timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
 \brief Compute the time difference t2-t1 between two times represented as timeval structs.
 \param result Time difference t2-t1
 \param t2 First time value
 \param t1 Second time value
 \return One if difference is negative, zero otherwise.
 */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);

#endif
