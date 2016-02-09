/*
 * Neuromapp - algos.hpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/spike/algos.hpp
 * contains algorithm definitions for SpikeExchangeGraph
 */

#include <assert.h>
#include <cstddef>
#include <stdlib.h>
#include <stddef.h>

#include "spike/mpispikegraph.h"

#ifndef algos_h
#define algos_h

void spike_exchange(MpiSpikeGraph &g){
    g.load_send_buf();
    //gather how many events each neighbor is sending
    g.allgather();
    //get the displacements
    g.set_displ();
    //next distribute items to every other process using allgatherv
    g.allgatherv();
}
#endif
