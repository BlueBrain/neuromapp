/*
 * Neuromapp - pool.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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

/**
 * @file neuromapp/coreneuron_1.0/queueing/pool.h
 * \brief Contains pool class declaration.
 */

#ifndef MAPP_POOL_H_
#define MAPP_POOL_H_

#include "coreneuron_1.0/queueing/thread.h"
#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace queueing {

class pool {
private:
    bool perform_algebra_;
    std::vector<nrn_thread_data> thread_datas_;

public:

    /** \fn pool(bool algebra)
     *  \brief initializes a pool with a thread_datas_ array
     *  \param algebra determines whether to perform linear algebra calculations
     */
    explicit pool(bool algebra=false);

    /** \fn ~pool()
     *  \brief accumulates statistics from the threadData array and stores them using impl::storage
     */
    ~pool();

    /** \fn send_events(int myID)
     *  \brief sends event to it's destination
     *  \param myID the thread index
     *  \precond generateAllEvents has been called
     *  \postcond thread_datas_[myID].generated_events size -= 1
     *  \return the time of the top element
     */
    double send_events(int myID);

    /** \fn void filter()
     *  \brief filters out relevent events(using the function matches()),
     *  and randomly selects a destination cellgroup, and delivers them
     *  using a no-lock inter_thread_send
     */
    void filter();

    void fixed_step(){
};

}
#endif
