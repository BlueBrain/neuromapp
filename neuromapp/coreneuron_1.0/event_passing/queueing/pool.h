/*
 * Neuromapp - pool.h, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/event_passing/queueing/pool.h
 * \brief Contains pool class declaration.
 */

#ifndef MAPP_POOL_H_
#define MAPP_POOL_H_

#include "coreneuron_1.0/event_passing/queueing/thread.h"
#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"
#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace queueing {

class pool {
private:
    bool perform_algebra_;
    int min_delay_;
    int time_;
    int rank_;
    int spike_stats_;
    spike::spike_interface& spike_;
    std::vector<nrn_thread_data> thread_datas_;

public:

    /** \fn pool(bool algebra, int ngroups, int min_delay, int rank, spike_interface& s_interface)
     *  \brief initializes a pool with a thread_datas_ array of size ngroups.
     *  \param algebra determines whether to perform linear algebra calculations
     *  \param ngroups the number of cell groups per node
     *  \param s_interface the spike interface used to communicate
     *  with the spike exchange algos
     */
    pool(bool algebra, int ngroups,int md, int rank, spike::spike_interface& s_interface):
    perform_algebra_(algebra), min_delay_(md), rank_(rank), spike_(s_interface),
    time_(0), spike_stats_(0) {thread_datas_.resize(ngroups);}

    /** \fn send_events(int myID, event_generator& generator)
     *  \brief sends event to it's destination
     *  \param myID the thread index
     *  \param generator the event generator from which events are received
     *  \precond generator has been initialized
     */
    template <typename G, typename P>
    void send_events(int myID, G& generator, P& presyns);

    /** \fn void fixed_step(event_generator& generator)
     *  \brief performs (min_delay_) iterations of a timestep in which:
     *      - events are sent
     *      - events are enqueued
     *      - events are delivered
     *      - linear algebra is performed
     *  \param generator the event generator from which events are received
     */
    template <typename G, typename P>
    void fixed_step(G& generator, P& presyns);

    /** \fn void filter(presyn_maker& presyns)
     *  \brief filters out relevent events(using the function matches()),
     *  and randomly selects a destination cellgroup, and delivers them
     *  using a no-lock inter_thread_send
     *  \param presyns the presyn maker from which input presyn information
     *  is gathered (used to distribute spike events between cell groups).
     */
    template <typename P>
    void filter(P& presyns);


    /** \fn accumulate_stats()
     *  \brief accumulate statistics from the threadData array and store them in spike_interface
     */
    void accumulate_stats();

//GETTERS
    /** \fn get_ngroups()
     *  \return the number of cellgroups
     */
    inline int get_ngroups() const { return thread_datas_.size(); }

    /** \fn get_time()
     * \return the current time_ value for this pool
     */
    inline int get_time() const { return time_; }
};

} //end of namespace

#include "coreneuron_1.0/event_passing/queueing/pool.ipp"

#endif
