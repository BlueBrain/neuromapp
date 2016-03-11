#include "coreneuron_1.0/environment/sim_env.h"
#include "coreneuron_1.0/environment/generator.h"
#include "coreneuron_1.0/environment/presyns.h"

void main1(int argc, char** argv){
    //set up environment
    sim_env environment;
    environment.num_groups_ = atoi(argv[1]);
    environment.sim_time_ = atoi(argv[2]);
    environment.num_procs_ = atoi(argv[3]);
    environment.rank_ = atoi(argv[4]);
    environment.time_ = 0;

   int num_out = atoi(argv[5]);
   int num_in = atoi(argv[6]);
   int netcons_per = atoi(argv[7]);

    presyn_maker presyns(num_out, num_in, netcons_per);
    presyns(environment);

    event_generator gen;
    gen(environment, presyns);
    stats_accumulator stats;

    nin_.resize(environment.num_procs_);
    displ_.resize(environment.num_procs_);
}
