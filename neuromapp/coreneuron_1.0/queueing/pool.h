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
 * @file neuromapp/coreneuron_1.0/queueing/pool.h
 * \brief Contains pool class declaration.
 */

#ifndef MAPP_POOL_H_
#define MAPP_POOL_H_

#include "coreneuron_1.0/queueing/thread.h"
#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace queueing {

class pool {
private:
#ifdef _OPENMP
    mapp::omp_lock spike_lock_;
#else
    mapp::dummy_lock spike_lock_;
#endif

    int num_cells_;
    int events_per_step_;
    int netcons_per_input_;
    int percent_ite_;
    int percent_spike_;
    bool v_;
    bool perform_algebra_;
    int time_;
    int spike_events_;
    const static int min_delay_ = 5;

    //needed for spike interface
    int num_out_;
    int num_in_;
    int num_procs_;
    int rank_;
    int total_received_;
    int total_relevent_;

    std::vector<nrn_thread_data> thread_datas_;

public:
    std::vector<event> spikein_;
    std::vector<event> spikeout_;
    std::map<int, std::vector<int> > input_presyns_;
    std::vector<int> output_presyns_;
    std::vector<int> nin_;
    std::vector<int> displ_;

    /** \fn pool(int numCells, int eventsPer, int pITE, bool verbose, bool algebra
     * int pSpike, int out, int in, int procs, int rank)
     *  \brief initializes a pool with a thread_datas_ array
     *  \param verbose verbose mode: 1 = on, 0 = off
     *  \param events_per_step_ number of events per time step
     *  \param percent_ite_ is the percentage of inter-thread events
     *  \param isSpike determines whether or not there are spike events
     *  \param algebra determines whether to perform linear algebra calculations
     */
    explicit pool(int numCells=1, int eventsPer=1, int pITE=0,
    bool verbose=false, bool algebra=false, int pSpike=0,
    int out=1, int in=1, int nc=1, int procs=1, int rank=0);

    /**
     * \fn void set_displ()
     * \brief fills the displacement container based on the sizes
     *  in nin that were received from allgather.
     *  displ will be used by the allgatherv function.
     */
    void set_displ();

    /** \fn accumulate_stats()
     *  \brief accumulates statistics from the threadData array and stores them using impl::storage
     */
    void accumulate_stats();

    /** \fn void generate_all_events(int totalTime)
     *  \brief creates all events for each thread that will be sent
     *   and received during the simulation lifetime.
     *  these are stored in the vector generated_events[] for each thread
     *  \param totalTime provides the total simulation time
     *  \postcond all events for the simulation are generated
     */
    void generate_all_events(int totalTime);

    /** \fn send_events(int myID)
     *  \brief sends event to it's destination
     *  \param myID the thread index
     *  \precond generateAllEvents has been called
     *  \postcond thread_datas_[myID].generated_events size -= 1
     */
    void send_events(int myID);

    /** \fn void filter()
     *  \brief filters out relevent events(using the function matches()),
     *  and randomly selects a destination cellgroup, and delivers them
     *  using a no-lock inter_thread_send
     */
    void filter();

    /** \fn int create_event(int myID, int curTime, int totalTime)
     *  \brief randomly generates a new event with data dependent on the values of
     *  percent-ite and percent-spike
     *  \param myID the thread index
     *  \param curTime the current time
     *  \param totalTime the total simulation time
     *  \return new_event
     */
    gen_event create_event(int myID, int curTime, int totalTime);

    /* \fn increment_time()
     * \brief increments the time_ counter
     */
    void increment_time(){++time_;}

    /* \fn mindelay()
     * \return the value of min_delay_
     */
    int mindelay() const {return min_delay_;}

    /* \fn cells()
     * \return the value of num_cells_
     */
    int cells() const {return num_cells_;}

    /* \fn received()
     * \return the value of total_received_
     */
    int received() const {return total_received_;}

    /* \fn relevent()
     * \return the value of total_relevent_
     */
    int relevent() const {return total_relevent_;}



//PARALLEL FUNCTIONS
    /** \fn void time_step()
     *  \brief master function to call parallel
     *   send, enqueue, algebra, and deliver
     */
    void time_step();

    /** \fn void parallel_send()
     *  \brief calls parallel send for every cellgroup
     */
    void parallel_send();

    /** \fn void parallel_enqueue()
     *  \brief calls parallel enqueue for every cellgroup
     */
    void parallel_enqueue();

    /** \fn void parallel_algebra()
     *  \brief calls parallel algebra for every cellgroup
     */
    void parallel_algebra();

    /** \fn void parallel_deliver()
     *  \brief calls parallel deliver for every cellgroup
     */
    void parallel_deliver();
};

}
#endif
