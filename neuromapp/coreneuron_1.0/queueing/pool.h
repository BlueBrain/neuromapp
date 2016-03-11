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
    int netcons_per_input_;
    int num_local_;
    int num_ite_;
    int num_spike_;
    double sim_time_;
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
    int total_relevant_;

    std::vector<nrn_thread_data> thread_datas_;

public:
    std::vector<event> spikein_;
    std::vector<event> spikeout_;
    std::map<int, std::vector<int> > input_presyns_;
    std::vector<int> output_presyns_;
    std::vector<int> nin_;
    std::vector<int> displ_;

    /** \fn pool(int numCells, int nLocal, int nIte, bool verbose, bool algebra
     * int nSpike, int out, int in, int procs, int rank)
     *  \brief initializes a pool with a thread_datas_ array
     *  \param numCells number of cell groups
     *  \param nLocal number of local events
     *  \param nIte number of ite events
     *  \param simTime the run time of the simulation
     *  \param verbose verbose mode: 1 = on, 0 = off
     *  \param algebra determines whether to perform linear algebra calculations
     *  \param nSpike number of spike events
     *  \param out the number of output presyns
     *  \param in the number of input presyns
     *  \param procs the number of processes
     *  \param rank the rank of the current process
     */
    explicit pool(int numCells=1, int nLocal=0, int nIte=0,
    int simTime=0, bool verbose=false, bool algebra=false, int nSpike=0,
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


    /** \fn calculate_probs(double& lambda, double* cdf)
     *  \brief updates the values of lambda and cdf based on the number
     *  of events of each type (spike, ite, local), and the time
     */
    void calculate_probs(double& lambda, double* cdf);

    /** \fn void generate_all_events()
     *  \brief creates all events for each thread that will be sent
     *   and received during the simulation lifetime.
     *  these are stored in the vector generated_events[] for each thread
     *  \postcond all events for the simulation are generated
     */
    void generate_all_events();

    /** \fn send_events(int myID)
     *  \brief sends event to it's destination
     *  \param myID the thread index
     *  \precond generateAllEvents has been called
     *  \postcond thread_datas_[myID].generated_events size -= 1
     *  \return the time of the top element
     */
    double send_events(int myID);

    /** \fn void filter()
     *  \brief filters out relevent events(using the function matches()),
     *  and randomly selects a destination cellgroup, and delivers them
     *  using a no-lock inter_thread_send
     */
    void filter();

    /* \fn simtime()
     * \return the value of sim_time_
     */
    int simtime() const {return sim_time_;}

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
    int relevant() const {return total_relevant_;}
};

}
#endif
