/*
 * Neuromapp - evironment.h, Copyright (c), 2015,
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
 * @file neuromapp/spike/environment.h
 * contains declaration for dummy environment class
 */
#ifndef enviro_h
#define enviro_h
#include <stdlib.h>
#include <stddef.h>
#include <vector>
#include <unistd.h>

#include "spike/algos.hpp"

namespace spike {

typedef std::vector<int> int_vec;
typedef std::vector<spike_item> spike_vec;

/**
 * Dummy environment. It's a simplified copy of the pool class from queueing
 * miniapp, containing the necessary data containers and functions used by
 * the spike exchange algorithms.
 *
 * Used to test algorithms.
 */
struct environment {
private:
    int events_per_;
    int num_out_;
    int num_in_;
    int num_procs_;
    int rank_;
    int total_received_;
    int total_relevent_;
    static const int cell_groups_ = 1;
    static const int min_delay_ = 5;

public:
    /*
     * containers used by spike exchange algorithms
     */
    spike_vec generated_spikes_;
    spike_vec spikein_;
    spike_vec spikeout_;
    int_vec nin_;
    int_vec displ_;
    int_vec input_presyns_;
    int_vec output_presyns_;

    /**
     * \fn environment(int e, int o, int i, int p, int r)
     * \brief environment constructor. Initializes member variables.
     */
    environment(int e, int o, int i, int p, int r);

    /**
     * \fn void time_step()
     * \brief prepares spikeout for allgather/allgatherv.
     */
    void time_step();

    /**
     * \fn void set_displ()
     * \brief fills the displacement container based on the sizes
     *  in nin that were received from allgather.
     *  displ will be used by the allgatherv function.
     */
    void set_displ();

    /**
     * \fn void generate_all_events(int totalTime)
     * \brief creates all events that will be used during the simulation
     */
    void generate_all_events(int totalTime);

    /**
     * \fn bool matches(const spike_item& sitem)
     * \brief compares the destination of sitem to the input_presyns
     * \returns true on a positive match, else false
     */
    bool matches(const spike_item& sitem);

    /**
     * \fn void time_step()
     * \brief iterates through spikein and calls matches() for every spike
     *  in the container.
     */
    int filter();

    /**
     * \fn void time_step()
     * \brief empty function that mirrors pool::increment_time()
     */
    void increment_time(){}

//GETTERS
    int mindelay(){return min_delay_;}
    int cells(){return cell_groups_;}
    int received(){return total_received_;}
    int relevent(){return total_relevent_;}

//TASKS
    void parallel_send();
    void parallel_enqueue(){usleep(10);}
    void parallel_algebra(){usleep(10);}
    void parallel_deliver(){usleep(10);}
};

}
#endif
