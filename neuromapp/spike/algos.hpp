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

/** \fn allgather(g)
    \brief performs a generic allgather on the data provided
    \param g the graph the operation is performed on
    \postcond g.sizeBuf is filled with sizes
  */
template<typename graph>
void allgather(graph &g){
    g.allgather();
}

/** \fn allgatherv(g)
    \brief performs a generic allgatherv on the data provided
    \param g the graph the operation is performed on
    \precond g.sendBuf and g.sizeBuf are not empty
    \postcond g.recvBuf is filled with elements
  */
template<typename graph>
void allgatherv(graph &g){
    g.allgatherv();
}


#endif
