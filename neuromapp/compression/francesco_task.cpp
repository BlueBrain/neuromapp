/*
 * Neuromapp - block.h, Copyright (c), 2015,
 * Devin Bayly - University of Arizona
 * baylyd@email.arizona.edu,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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

#include <iostream>
#include <chrono>
#include <cmath>
#include <fstream>
#include <bitset>
#include "compression/compression.h"
#include "neuromapp/compression/common/data/path.h"
#include "compression/block.h"
#include "compression/block.hpp"
#include "compression/timer_tool.h"
using namespace std;


using neuromapp::block;
using neuromapp::Timer;
using neuromapp::cstandard;
typedef double value_type;
typedef size_t size_type;
typedef double * pointer;
using expmt_block = block<value_type,cstandard>;

/*this function aims to take in a upper limit for time, and return a bitset representing the presence or absence of a spike at that time according to a poisson process */
/**
 * create_spike_time 
 *
 *
 * @brief
 *
 * @param int intensity
 *
 * @return double
 */
double create_spike_time(int intensity) {
    // must be an inverse exponential 
    double rand_val = (double) rand()/RAND_MAX;// subtracting from 1.0 makes sure we don't get a zero
    return -log(1.0- rand_val)/intensity;
}

/**
 * calc_u 
 *
 *
 * @brief
 *
 * @param double u,double fraction,double d_time,double time_const_fac
 *
 * @return double
 */
double calc_u(double u,double fraction,double d_time,double time_const_fac) {
    return fraction + u*(1-fraction)*exp(-d_time/time_const_fac);
}

/**
 * calc_x 
 *
 *
 * @brief
 *
 * @param double x,double u,double d_time,double time_const_rec
 *
 * @return double
 */
double calc_x(double x,double u,double d_time,double time_const_rec) {
    return 1+(x - x*u - 1)*exp(-d_time/time_const_rec);
}

/**
 * run_workflow 
 *
 *
 * @brief
 *
 * @param 
 *
 * @return void
 */
void run_workflow() {
    //create the blocks
    expmt_block ro_block,ux_block;
    ifstream read_only_file(mapp::path_specifier::give_path() +"block_data/values_8_a10261trans_bulk.csv");
    ifstream dynamic_file(mapp::path_specifier::give_path() +"block_data/dynamic_block.csv");
    read_only_file >> ro_block;
    dynamic_file >> ux_block;
    //close the files
    read_only_file.close();
    dynamic_file.close();
    //cols is the same as # of neurons
    size_type cols_ = ro_block.dim0();
    //compress the blocks 
    //ro_block.compress();
    //ux_block.compress();
    //create the neuron time vector
    vector<double> neuron_times(cols_,0.0) ;
    double max_time = 100.0;
    //start experiment by iterating through neuron ids
    //time the experiment
    Timer time_it;
    time_it.start();
    for (size_type i = 0 ;i < cols_;i++) {
        // all time values for neurons
        int nth_spike = 0;
        while (neuron_times[i] < max_time) {
            //uncompress blocks
            //ro_block.uncompress();
            //ux_block.uncompress();
            //get last u,x
            double u = ux_block(i,1),x = ux_block(i,0);
            //get new spike time
            double intensity = 1.0;
            double delta_t = create_spike_time(intensity);
            //calculate new u,x
            ux_block(i,1) = calc_u(u,ro_block(i,3),delta_t,ro_block(i,1));
            ux_block(i,0) = calc_x(x,u,delta_t,ro_block(i,2));
            //update the neuron time, and loop
            neuron_times[i] += delta_t;
            double PSP = ro_block(i,0)*ux_block(i,1)*ux_block(i,0); 
            nth_spike++;
            //do printout of calc
            std::cout << "neuron: " << i << " time: " << neuron_times[i]
                << " old u,x " << u << "," << x 
                << " new u,x " << ux_block(i,1) << "," << ux_block(i,0)
                << " PSP is " << PSP << " for " << nth_spike << "th spike "<< std::endl;
            //recompress the blocks
            //ro_block.compress();
            //ux_block.compress();
        }
    }
    time_it.end();
    std::cout << "experiment took " << time_it.duration() << " (ms) to run" << std::endl;
}

/**
 * main 
 *
 *
 * @brief
 *
 * @param int argc,char ** argv
 *
 * @return int
 */
int main (int argc,char ** argv) {
    run_workflow();
}
