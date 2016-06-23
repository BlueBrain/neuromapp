namespace environment {

template< typename Iterator >
void generate_events_kai(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda) {

    int dest = 0;
    double event_time = 0;
    int src_gid = 0;
    gen_event new_event;

    int cells_per = ncells / nprocs;
    int start = rank * cells_per;

    //for the last rank, add the remaining cellgroups
    if(rank == (nprocs - 1))
	cells_per = ncells - start;

    //create random number generator/distributions
    /*
    * rng       => random number generator
    * time_d    => exponential distribution of event times
    * gid_d     => uniform distribution to create indices for output gids
    * percent_d => uniform distribution to decide event type based on percent
    */

    boost::mt19937 rng(rank + time(NULL));
    boost::random::exponential_distribution<double> time_d(lambda);
    boost::random::uniform_int_distribution<> gid_d(start, (start + cells_per - 1));

    event_time = 0;
    Iterator it;
    //create events up until simulation end
    while(event_time < simtime){
	double diff = time_d(rng);
	assert(diff > 0.0);
	event_time += diff;
	if(event_time >= simtime){
	    break;
	} else {
	    src_gid = gid_d(rng);

	    //cellgroups are determined by:
	    //group # = gid % number of groups
	    dest = src_gid % ngroups;

	    new_event.first = src_gid;
	    new_event.second = static_cast<int>(event_time);

	    it = beg;
            std::advance(it, dest);
	    it->push(new_event);
	}
    }
}

template< typename Iterator >
void generate_poisson_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda) {

    int dest = 0;
    double event_time = 0;
    int src_gid = 0;
    gen_event new_event;

    int cells_per = ncells / nprocs;
    int start = rank * cells_per;

    //for the last rank, add the remaining cellgroups
    if(rank == (nprocs - 1))
	cells_per = ncells - start;

    //create random number generator/distributions
    /*
    * rng       => random number generator
    * time_d    => exponential distribution of event times
    * gid_d     => uniform distribution to create indices for output gids
    * percent_d => uniform distribution to decide event type based on percent
    */

    boost::mt19937 rng(rank + time(NULL));
    boost::random::poisson_distribution<int> event_d(lambda);
    boost::random::uniform_int_distribution<> gid_d(start, (start + cells_per - 1));

    event_time = 0;
    Iterator it;
    int event_num;
    //create events up until simulation end
    for(event_time = 0; event_time < simtime; ++event_time){
	event_num = event_d(rng);
	if(event_num >= cells_per) event_num = cells_per; // ensures you don't have more events than c    ells, although this is not statistically correct...

	for( int event_count = 0; event_count < event_num; ++event_count) {
	    src_gid = gid_d(rng);

	    //cellgroups are determined by:
	    //group # = gid % number of groups
	    dest = src_gid % ngroups;

	    new_event.first = src_gid;
	    new_event.second = static_cast<int>(event_time);

	    it = beg;
            std::advance(it, dest);
	    it->push(new_event);
	}
    }
}


template< typename Iterator >
void generate_uniform_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double firing_interval) {

    int dest = 0;
    double event_time = 0;
    int src_gid = 0;
    gen_event new_event;

    int cells_per = ncells / nprocs;
    int start = rank * cells_per;

    //for the last rank, add the remaining cellgroups
    if(rank == (nprocs - 1))
	cells_per = ncells - start;

    event_time = 0;
    Iterator it;
    int event_num;
    //create events up until simulation end
    while(event_time < simtime){
	event_time += firing_interval;
	if(event_time >= simtime){
	    break;
	} else {
	    for(src_gid = start; src_gid < start + cells_per; ++src_gid) {

	    //cellgroups are determined by:
	    //group # = gid % number of groups
	    dest = src_gid % ngroups;

	    new_event.first = src_gid;
	    new_event.second = static_cast<int>(event_time);

	    it = beg;
            std::advance(it, dest);
	    it->push(new_event);

	    }
	}
    }
}



} // close namespace environment
