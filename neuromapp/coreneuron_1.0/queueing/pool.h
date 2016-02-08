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

#include <boost/array.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <numeric>

#include "coreneuron_1.0/queueing/thread.h"
#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef MAPP_POOL_H_
#define MAPP_POOL_H_

namespace queueing {

template<implementation I>
class nrn_thread_data;

template<implementation I>
class pool {
private:
#ifdef _OPENMP
    omp_lock spike_lock_;
#else
    dummy_lock spike_lock_;
#endif

    int time_;
    bool v_;
    bool perform_algebra_;
    int percent_ite_;
    int percent_spike_;
    int events_per_step_;
    int all_spiked_;
    int cell_groups_;
    const static int min_delay_ = 5;

    std::vector<nrn_thread_data<I> > thread_datas_;
    std::vector<event> spike_in_;
    std::vector<event> spike_out_;

public:
    /** \fn pool(bool verbose, int eventsPer, int pITE, bool isSpike, bool algebra)
     *  \brief initializes a pool with a thread_datas_ array
     *  \param verbose verbose mode: 1 = on, 0 = off
     *  \param events_per_step_ number of events per time step
     *  \param percent_ite_ is the percentage of inter-thread events
     *  \param isSpike determines whether or not there are spike events
     *  \param algebra determines whether to perform linear algebra calculations
     */
    explicit pool(int numCells=0, int eventsPer=0, int pITE=0, int pSpike=0,
    bool verbose=false, bool algebra=false):
    cell_groups_(numCells), events_per_step_(eventsPer),
    percent_ite_(pITE), percent_spike_(pSpike),
    v_(verbose), perform_algebra_(algebra),
    all_spiked_(0), time_(0)
    {
        srand(time(NULL));
        thread_datas_.resize(cell_groups_);
    }

    /** \fn accumulate_stats()
     *  \brief accumulates statistics from the threadData array and stores them using impl::storage
     */
    void accumulate_stats(){
        int all_ite_received = 0;
        int all_enqueued = 0;
        int all_delivered = 0;
        for(int i=0; i < thread_datas_.size(); ++i){
            all_ite_received += thread_datas_[i].ite_received_;
            all_enqueued += thread_datas_[i].enqueued_;
            all_delivered += thread_datas_[i].delivered_;
            if(v_){
                std::cout<<"Cellgroup "<<i<<" ite received: "
                <<thread_datas_[i].ite_received_<<std::endl;
                std::cout<<"Cellgroup "<<i<<" enqueued: "<<
                thread_datas_[i].enqueued_<<std::endl;
                std::cout<<"Cellgroup "<<i<<" delivered: "<<
                thread_datas_[i].delivered_<<std::endl;
            }
        }

        if(v_){
            std::cout<<"Total inter-thread received: "<<all_ite_received<<std::endl;
            std::cout<<"Total enqueued: "<<all_enqueued<<std::endl;
            std::cout<<"Total spiked: "<<all_spiked_<<std::endl;
            std::cout<<"Total delivered: "<<all_delivered<<std::endl;
        }
        neuromapp_data.put_copy("inter_received", all_ite_received);
        neuromapp_data.put_copy("enqueued", all_enqueued);
        neuromapp_data.put_copy("spikes", all_spiked_);
        neuromapp_data.put_copy("delivered", all_delivered);
    }

    /** \fn void time_step()
     *  \brief master function to call generate, enqueue, and deliver
     */
    void time_step(){
        int size = thread_datas_.size();
        #pragma omp parallel for schedule(static,1)
        for(int i = 0; i < size; ++i){
            send_events(i);

            //Have threads enqueue their interThreadEvents
            thread_datas_[i].enqueue_my_events();

            if(perform_algebra_)
                thread_datas_[i].l_algebra(time_);

                        /// Deliver events
            while(thread_datas_[i].deliver(i, time_));

        }
        time_++;
    }

    /** \fn void generate_all_events(int totalTime)
     *  \brief creates all events for each thread that will be sent
     *   and received during the simulation lifetime.
     *  these are stored in the vector generated_events[] for each thread
     *  \param totalTime provides the total simulation time
     *  \postcond all events for the simulation are generated
     */
    void generate_all_events(int totalTime){
        event ev;
        for(int i = 0; i < cell_groups_; ++i){
            for(int j = 0; j < totalTime; ++j){
                /// Simulated target of a NetCon and the event time
                for(int k = 0; k < events_per_step_; ++k){
                    //events can be generated with time range: (current time) to (current time + 10%)
                    ev = create_event(i,j,totalTime);
                    thread_datas_[i].push_generated_event(ev.data_, ev.t_, ev.is_spike_);
                }
            }
        }

        //generate the spike_in_ events_
        int dst = 0;
        int num_spikes = totalTime*cell_groups_*events_per_step_*percent_spike_/100;
        for(int i = 0; i < num_spikes; ++i){
            int diff = 1;
            if(totalTime > 10)
                diff = rand() % (totalTime/10);

            ev.t_ = static_cast<double>(time_ + diff + min_delay_);
            dst = rand() % thread_datas_.size();
            ev.data_ = static_cast<double>(dst);
			spike_in_.push_back(ev);
        }
    }

    /** \fn send_events(int myID)
     *  \brief sends event to it's destination
     *  \param myID the thread index
     *  \precond generateAllEvents has been called
     *  \postcond thread_datas_[myID].generated_events size -= 1
     */
    void send_events(int myID){
        for(int i = 0; i < events_per_step_; ++i){
            event e = thread_datas_[myID].pop_generated_event();
            int dst_nt = static_cast<int>(e.data_);
            //if spike event send to spike_out
            if(e.is_spike_){
                spike_lock_.acquire();
                spike_out_.push_back(e);
                spike_lock_.release();
            }
            //if destination id is my own, self event, else ite
            else if(dst_nt == myID)
                thread_datas_[dst_nt].self_send(e.data_, e.t_);
            else
                thread_datas_[dst_nt].inter_thread_send(e.data_, (e.t_ + min_delay_));
        }
    }

    /** \fn void handleSpike(int totalTime)
     *  \brief compensates for the spike exchange by adding events every 5 timesteps
     *  \param totalTime tells the provides the total simulation time
     */
    void handle_spike(int totalTime){
        if( (time_ % min_delay_) == 0){
            //"Send" spikes
            spike_lock_.acquire();
            spike_out_.clear();
            spike_lock_.release();

            //"Receive" spikes
            event ev;
            int num_spikes = min_delay_*cell_groups_*events_per_step_*percent_spike_/100;
            for(int i = 0; i < num_spikes; ++i){
                ev = spike_in_.back();
                spike_in_.pop_back();
                int dst = static_cast<int>(ev.data_);
                thread_datas_[dst].self_send(ev.data_, ev.t_);
                all_spiked_++;
            }
        }
    }

    /** \fn int create_event(int myID, int curTime, int totalTime)
     *  \brief randomly generates a new event with data dependent on the values of
     *  percent-ite and percent-spike
     *  \param myID the thread index
     *  \param curTime the current time
     *  \param totalTime the total simulation time
     *  \return new_event
     */
    event create_event(int myID, int curTime, int totalTime){
        event new_event;
        int diff = 1;
        if(totalTime > 10)
               diff = rand() % (totalTime/10);

        //set time_ to be some time in the future t + diff
        new_event.t_ = static_cast<double>(curTime + diff);

        int dst = myID;
        int percent = rand() % 100;
        if (percent < percent_spike_){
            //send as spike
            //dst = ???;        //should we have a list of output presyn gids?
            new_event.is_spike_ = true;
        }
        else if(percent < (percent_spike_ + percent_ite_)){
            //send as inter_thread_event_
            while(dst == myID)
                dst = rand() % thread_datas_.size();
        }
        //else self send
        new_event.data_ = static_cast<double>(dst);
        return new_event;
    }
};

}
#endif
