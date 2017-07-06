
#include <iostream>
#include <chrono>
#include <cmath>
#include <fstream>
#include <bitset>
#include "compression.h"
#include "block.h"
#include "bit_shifting.h"
using namespace std;


using neuromapp::block;
using neuromapp::cstandard;
typedef float value_type;
typedef float * pointer;
using prac_block = block<value_type,cstandard>;

/*this function aims to take in a upper limit for time, and return a bitset representing the presence or absence of a spike at that time according to a poisson process */
double create_spike_time(int intensity) {
    // must be an inverse exponential 
    double rand_val = (double) rand()/RAND_MAX;// subtracting from 1.0 makes sure we don't get a zero
    std::cout << "rand val is " << rand_val << std::endl;
    return -log(1.0- rand_val)/intensity;
}

void make_spike_record(int experiment_length) {
    srand(time(NULL));
    vector<double> spike_record;
    double spike_time = 0;
    int count = 0;
    int intensity = 1;
    while (spike_time < experiment_length) {
        spike_time +=create_spike_time(intensity);
        count++;
        spike_record.push_back(spike_time);
    }
    //TODO think about writing this to a file
}
    
int main (void) {
}
