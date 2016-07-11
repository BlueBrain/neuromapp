/*
 * simulationmanager.cpp
 *
 *  Created on: Jul 6, 2016
 *      Author: schumann
 */

#include "nest/simulationmanager.h"
#include "nest/eventdelivermanager.h"

using namespace nest;

simulationmanager::simulationmanager(eventdelivermanager& edm, environment::event_generator& generator, const int rank, const int num_processes, const int num_threads):
    edm_(edm),
    generator_(generator),
    rank_(rank),
    num_processes_(num_processes),
    num_threads_(num_threads)
{
}

void
simulationmanager::update(const int thrd, const int t, long from_step, long to_step)
{
    //iterate over steps
    for (long lag=from_step; lag<to_step; lag++) {
        const int curTime=t+lag;
        while(generator_.compare_top_lte(thrd, curTime)) {
            environment::gen_event g = generator_.pop(thrd);
            spikeevent se;
            se.set_sender_gid(g.first);
            se.set_stamp(g.second);

            edm_.send_remote(thrd, se, lag);
        }
    }
}

