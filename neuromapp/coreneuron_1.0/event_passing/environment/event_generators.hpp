#ifndef MAPP_ENV_GENERATORS_HPP
#define MAPP_ENV_GENERATORS_HPP

/*
* Neuromapp - hines.c, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* francesco.cremonesi@epfl.ch
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
* @file neuromapp/coreneuron_1.0/event_passing/environment/event_generators.hpp
* \brief Implements several functions for generating spike events
*/

#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include <cassert>

#include "coreneuron_1.0/event_passing/environment/generator.h"

namespace environment {

    /** \fn void generate_events_kai(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda)
        \brief Kai's original implementation: generates events according to an exponential law.
	\param beg iterator to the beginning of the vector of queues of events.
	\param simtime total time of simulation (in number of timesteps)
	\param ngroups number of cellgroups per distributed rank
	\param rank distributed rank number
	\param nprocs the total number of distributed ranks in the whole simulation
	\param ncells the total number of cells in the whole simulation
	\param lambda the firing frequency of a single cell (in num events/timestep )

	This implementation generates an event per timestep, and attributes it to a random cell in the network. It then samples a survival interval using an exponential distribution to determine when the next event will occur. This function populates a vector of queues, whose beginning is obtained by the parameter beg. The vector of queues is structured as follows: at position beg + i, we have the queue of events whose source gid belongs to the i^th cellgroup.
	\warning this implementation lacks a biological implementation, since no two events can happen at the same time. This would imply that, in the whole network, only one cell fired at a time .
    */
    template< typename Iterator >
    void generate_events_kai(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda);

    /** \fn void generate_poisson_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda)
        \brief generates events based on a poisson distribution approximation.
	\param beg iterator to the beginning of the vector of queues of events.
	\param simtime total time of simulation (in number of timesteps)
	\param ngroups number of cellgroups per distributed rank
	\param rank distributed rank number
	\param nprocs the total number of distributed ranks in the whole simulation
	\param ncells the total number of cells in the whole simulation
	\param lambda the firing frequency of a single cell (in num events/timestep )

	This implementation draws, at each timestep, a number of events from a Poisson distribution (see e.g. <a href="https://en.wikipedia.org/wiki/Poisson_distribution"> the Wikipedia page</a>). It distributes these events randomly among cells, without keeping memory of the history of each cell. There is no refractory period.
*/
    template< typename Iterator >
    void generate_poisson_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda);

    /** \fn void generate_uniform_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double firing_interval)
        \brief generates events based on a poisson distribution approximation.
	\param beg iterator to the beginning of the vector of queues of events.
	\param simtime total time of simulation (in number of timesteps)
	\param ngroups number of cellgroups per distributed rank
	\param rank distributed rank number
	\param nprocs the total number of distributed ranks in the whole simulation
	\param ncells the total number of cells in the whole simulation
	\param firing_interval the interval between spikes (in timesteps)

	In this implementation, _all_ the cells fire at every multiple of the firing interval.
	\warning firing_interval must be strictly positive. In debug mode, this is asserted.
*/
    template< typename Iterator >
    void generate_uniform_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, int firing_interval);

}

#include "coreneuron_1.0/event_passing/environment/event_generators.ipp"

#endif
