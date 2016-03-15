#include "coreneuron_1.0/event_passing/environment/environment.h"
#include "coreneuron_1.0/event_passing/environment/event_generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/queueing/pool.h"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"
#include "coreneuron_1.0/event_passing/spike/algos.hpp"

int main(int argc, char** argv){
    if(argc != 2){
        exit(EXIT_FAILURE);
    }
    //filename passed as command line arg
    std::ifstream constraint_file(argv[1]);
    int ngroups;
    int simtime;
    int out;
    int in;
    int netconsper;
    int nSpikes;
    int nIte;
    int nLocal;

    //rankd and nprocs can be determined using MPI calls
    //using dummies for now
    int nprocs = 1;
    int rank = 0;
    bool algebra = false;
    myfile >> ngroups;
    myfile >> simtime;

    //create presyn_maker
    myfile >> out;
    myfile >> in;
    myfile >> netconsper;
    environment::presyn_maker presyns(out, in, netconsper);

    //create event generator
    myfile >> nSpikes;
    myfile >> nIte;
    myfile >> nLocal;
    environment::event_generator generator(nSpikes, nIte, nLocal);

    //generate presyns and events
    presyns(nprocs, ngroups, rank);
    generator(simtime, ngroups, rank, presyns);

    /*
     * Run the actual solver:
     * Queueing and Spike Exchange
     */

    spike::spike_interface s_interface(nprocs);
    queueing::pool pl(algebra, ngroups, s_interface);

    while(pl.get_time() < simtime){
        pl.fixed_step(generator);
        spike_exchange(s_interface);
        pl.filter(presyns);
    }

    return 0;
}



