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

#include <iostream>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "spike/algos.hpp"

namespace spike {

typedef std::vector<int> int_vec;
typedef std::vector<spike_item> spike_vec;
typedef boost::mt19937 rng;
typedef boost::random::uniform_int_distribution<> uniform;
typedef boost::random::exponential_distribution<double> exp;


/**
 * Random number generator wrapper. Used to create random numbers with a
 * uniform and exponential distribution.
 *
 */
struct random_gen {
private:
    rng rng_;
    uniform uni_;
    exp exp_;

public:

    /**
     * \fn environment(int e, int ns, int o, int i, int nc, int p, int r)
     * \brief environment constructor. Initializes member variables.
     * \param seed unique seed to generate random numbers
     * \param max the maximum value in the uniform range
     * \param lambda the number of spikes divided by the simulation time
     *  used to generate an exponential distribution
     */
    explicit random_gen(int seed=0, int max=1, double lambda=2):
    rng_(seed), uni_(0, max), exp_(lambda) {}

    int gen_uni(){return uni_(rng_);}
    double gen_exp(){return exp_(rng_);}
};


/**
 * Dummy environment. It's a simplified copy of the pool class from queueing
 * miniapp, containing the necessary data containers and functions used by
 * the spike exchange algorithms.
 *
 * Used to test algorithms.
 */
struct environment {
private:
    int num_out_;
    int num_in_;
    int netcons_per_input_;
    int num_procs_;
    int rank_;
    int time_;
    int num_spikes_;
    int sim_time_;
    int total_received_;
    int total_relevant_;
    static const int num_cells_ = 1;
    static const int min_delay_ = 5;

    double exp_dist_lambda_;
    random_gen rng_;

public:
    /*
     * containers used by spike exchange algorithms
     */
    spike_vec generated_spikes_;
    spike_vec spikein_;
    spike_vec spikeout_;
    int_vec nin_;
    int_vec displ_;
    std::map<int, int_vec> input_presyns_;
    int_vec output_presyns_;

    /**
     * \fn environment(int e, int ns, int o, int i, int nc, int p, int r)
     * \brief environment constructor. Initializes member variables.
     * \param ns total number of spikes
     * \param t simulation time
     * \param o output presyns per mpi task
     * \param i input presyns per mpi task
     * \param nc netcons per input presyn
     * \param p number of processes
     * \param r rank of this process
     */
    environment(int ns, int t, int o, int i, int nc, int p, int r);

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
     * \fn void generate_all_events()
     * \brief creates all events that will be used during the simulation
     */
    void generate_all_events();

    /**
     * \brief dummy function to match pool function
     */
    bool accumulate_stats() {};

    /**
     * \fn void filter()
     * \brief iterates through spikein and calls matches() for every spike
     *  in the container.
     */
    void filter();

    /**
     * \fn void intcrement_time()
     * \brief function that mirrors pool::increment_time()
     */
    void increment_time(){++time_;}

//GETTERS
    int mindelay() const {return min_delay_;}
    int cells() const {return num_cells_;}
    int received() const {return total_received_;}
    int relevant() const {return total_relevant_;}
    int simtime() const {return sim_time_;}

//TASKS
    void parallel_send();
    void parallel_enqueue(){usleep(10);}
    void parallel_algebra(){usleep(10);}
    void parallel_deliver(){usleep(10);}
};

}
#endif
