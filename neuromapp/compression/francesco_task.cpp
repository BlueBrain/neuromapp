/* Filename : francesco_task.cpp
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : xxx
 * Date : 2017-07-20 
 */

#include <iostream>
#include <chrono>
#include <cmath>
#include <fstream>
#include <bitset>
#include "compression.h"
#include "timer_tool.h"
#include "block.h"
using namespace std;


using neuromapp::block;
using neuromapp::Timer;
using neuromapp::cstandard;
typedef double value_type;
typedef size_t size_type;
typedef double * pointer;
using expmt_block = block<value_type,cstandard>;

/**
 * create_spike_time 
 *
 *
 * @brief this function aims to take in a upper limit for time, and return a bitset representing the presence or absence of a spike at that time according to a poisson process 
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
/*TODO make little description, and show snippet of ro_block structure
 * to help clarify index use below 
 */
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
    ifstream read_only_file("../compression/francesco_data/readonly_block2017-07-10-13:51.dat");
    ifstream dynamic_file("../compression/francesco_data/dynamic_block2017-07-10-13:52.dat");
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
