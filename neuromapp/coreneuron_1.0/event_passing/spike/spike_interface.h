/*
 * Neuromapp - spike_interface.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/coreneuron_1.0/event_passing/spike/spike_interface.h
 * \brief Contains spike_interface class declaration.
 */

#ifndef MAPP_SPIKE_INTERFACE_H
#define MAPP_SPIKE_INTERFACE_H

#include <iostream>

#include "utils/omp/lock.h"

namespace spike {

/**
    \brief spike_interface acts as the interface between spike exchange
    and queueing. Queueing stores events in spikeout_, which are
    communicated between processes by spike exchange (and stored in spikein_).
    Afterwards, events are retreived and processed by the queueing algo.
 */
struct spike_interface{
#ifdef _OPENMP
    mapp::omp_lock lock_;
#else
    mapp::dummy_lock lock_;
#endif

    //CONTAINERS
    std::vector<queueing::event> spikein_;
    std::vector<queueing::event> spikeout_;
    std::vector<int> nin_;
    std::vector<int> displ_;

    //STATS ACCUMULATORS
    int spike_stats_;
    int ite_stats_;
    int local_stats_;
    int post_spike_stats_;
    int received_spike_stats_;

    /** \fn spike_interface(int nprocs)
        \brief spike_interface constructor. Initializes nin and displ buffers
        to have size == number of processes
     */
    spike_interface(int nprocs):
        spike_stats_(0),
        ite_stats_(0),
        local_stats_(0),
        post_spike_stats_(0),
        received_spike_stats_(0)
        {nin_.resize(nprocs); displ_.resize(nprocs);}
};

} //end of namespace

#endif
