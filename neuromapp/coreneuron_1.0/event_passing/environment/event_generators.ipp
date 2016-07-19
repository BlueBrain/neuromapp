namespace environment {

template< typename Iterator >
void generate_events_kai(Iterator beg, int simtime, int ngroups, int rank, int nprocs, double lambda, neurondistribution* neuron_dist ) {

    gen_event new_event;

    //create random number generator/distributions
    /*
    * rng       => random number generator
    * time_d    => exponential distribution of event times
    * gid_d     => uniform distribution to create indices for output gids
    * percent_d => uniform distribution to decide event type based on percent
    */

    boost::mt19937 rng(rank + time(NULL));
    boost::random::exponential_distribution<double> time_d(neuron_dist->getglobalcells()*lambda/nprocs);

    //generate local ids
    boost::random::uniform_int_distribution<> lid_d(0, neuron_dist->getlocalcells()-1);

    double event_time = 0;
    Iterator it;
    //create events up until simulation end
    while(event_time < simtime){
	double diff = time_d(rng);
	assert(diff > 0.0);
	event_time += diff;
	if(event_time >= simtime){
	    break;
	} else {

	    // generate local id from random variable
	    const int src_lid = lid_d(rng);
	    //convert to global id
	    const int src_gid = neuron_dist->local2global(src_lid);

	    //cellgroups are determined by:
	    //group # = gid % number of groups
	    const int dest = src_gid % ngroups;

	    //concert local ids to global ids
	    new_event.first = src_gid;
	    new_event.second = static_cast<int>(event_time);

	    it = beg;
            std::advance(it, dest);
	    it->push(new_event);
	}
    }
}

template< typename Iterator >
void generate_poisson_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, double lambda, neurondistribution* neuron_dist) {

    gen_event new_event;

    //create random number generator/distributions
    /*
    * rng       => random number generator
    * time_d    => exponential distribution of event times
    * gid_d     => uniform distribution to create indices for output gids
    * percent_d => uniform distribution to decide event type based on percent
    */

    boost::mt19937 rng(rank + time(NULL));
    boost::random::poisson_distribution<int> event_d(neuron_dist->getglobalcells()*lambda/nprocs);
    boost::random::uniform_int_distribution<> lid_d(0, neuron_dist->getlocalcells()-1);

    double event_time = 0;
    Iterator it;
    int event_num;
    //create events up until simulation end
    for(event_time = 0; event_time < simtime; ++event_time){
	event_num = event_d(rng);
	if(event_num >= neuron_dist->getlocalcells()) event_num = neuron_dist->getlocalcells(); // ensures you don't have more events than cells, although this is not statistically correct...

	for( int event_count = 0; event_count < event_num; ++event_count) {
	    // generate local id from random variable
        const int src_lid = lid_d(rng);
        //convert to global id
        const int src_gid = neuron_dist->local2global(src_lid);

	    //cellgroups are determined by:
	    //group # = gid % number of groups
	    const int dest = src_gid % ngroups;

	    new_event.first = src_gid;
	    new_event.second = static_cast<int>(event_time);

	    it = beg;
            std::advance(it, dest);
	    it->push(new_event);
	}
    }
}


template< typename Iterator >
void generate_poisson_events_net(Iterator beg, const int& seed,  const int& simtime, const double& net_firing_rate, const neurondistribution& neuron_dist) { 
    gen_event new_event;

    if (simtime < 100)
	std::cout << "WARNING in generate_possion_events_seq: not enough picks to garanty randomness" << std::endl;

    //create random number generator/distributions
    boost::mt19937 rng(seed);
    boost::random::poisson_distribution<int> event_d(net_firing_rate);
    boost::random::uniform_int_distribution<> gid_d(0, neuron_dist.getglobalcells()-1);

    double event_time = 0;
    Iterator it;
    int event_num;
    //create events up until simulation end
    for(event_time = 0; event_time < simtime; ++event_time){
        event_num = event_d(rng);
        //if(event_num >= neuron_dist->getglobalcells()) event_num = neuron_dist.getglobalcells(); // ensures you don't have more events than cells, although this is not statistically correct...

        for( int event_count = 0; event_count < event_num; ++event_count) {
            // generate local id from random variable
            const int src_gid = gid_d(rng);
            if (neuron_dist.isLocal(src_gid)) {
                new_event.first = src_gid;
                new_event.second = static_cast<int>(event_time);
                beg->push(new_event);
            }
        }
    }
}

template< typename Iterator >
void generate_poisson_events_neuron(Iterator beg, const int& seed,  const int& simtime, const double& neuron_firing_rate, const neurondistribution& neuron_dist) { 
    gen_event new_event;

    //assert(neuron_firing_rate<1.0000001);

    //create random number generator/distributions
    boost::mt19937 rng(seed);
    boost::random::uniform_real_distribution<double> pick(0,1);
    boost::random::uniform_int_distribution<> gid_d(0, neuron_dist.getglobalcells()-1);

    double event_time = 0;
    Iterator it;
    int event_num;
    //create events up until simulation end
    for(event_time = 0; event_time < simtime; ++event_time){
        //if(event_num >= neuron_dist->getglobalcells()) event_num = neuron_dist.getglobalcells(); // ensures you don't have more events than cells, although this is not statistically correct...
        for( int lid = 0; lid < neuron_dist.getlocalcells(); lid++) {
            // generate local id from random variable
            //take rates>1 into account
            double r = neuron_firing_rate;
	    while (r>pick(rng)) {
                const int src_gid = neuron_dist.local2global(lid);
                new_event.first = src_gid;
                new_event.second = static_cast<int>(event_time);
                beg->push(new_event);
		r--;
            }
        }
    }
}


template< typename Iterator >
void generate_uniform_events(Iterator beg, int simtime, int ngroups, int firing_interval, neurondistribution* neuron_dist) {

    assert(firing_interval > 0);

    double event_time = 0;
    gen_event new_event;

    event_time = 0;
    Iterator it;
    //create events up until simulation end
    while(event_time < simtime){
	event_time += firing_interval;
	if(event_time >= simtime){
	    break;
	} else {
	    for(int src_lid = 0; src_lid < neuron_dist->getlocalcells(); ++src_lid) {
        //convert to global id
        const int src_gid = neuron_dist->local2global(src_lid);
	    //cellgroups are determined by:
	    //group # = gid % number of groups
	    const int dest = src_gid % ngroups;

	    new_event.first = src_gid;
	    new_event.second = event_time;

	    it = beg;
            std::advance(it, dest);
	    it->push(new_event);

	    }
	}
    }
}



} // close namespace environment
