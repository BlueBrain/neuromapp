/*
 * simulationmanager.h
 *
 *  Created on: Jul 6, 2016
 *      Author: schumann
 */

#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include "nest/nestkernel/event_passing/eventdelivermanager.h"
#include "coreneuron_1.0/event_passing/environment/event_generators.hpp"

namespace nest {
    class simulationmanager {
    private:
        eventdelivermanager& edm_;
        environment::event_generator& generator_;
        int rank_;
        int num_processes_;
        int num_threads_;
    public:
        simulationmanager(eventdelivermanager& edm, environment::event_generator& generator, const int, const int, const int);

        void update(const int thrd, const int t, long from_step, long to_step);
    };
};

#endif /* SIMULATIONMANAGER_H_ */
