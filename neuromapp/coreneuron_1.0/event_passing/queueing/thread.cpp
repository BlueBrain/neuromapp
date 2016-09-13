/*
 * Neuromapp - thread.cpp, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/event_passing/queueing/thread.cpp
 * \brief Contains nrn_thread_data class definition.
 */

#include <queue>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <utility>

#include "coreneuron_1.0/event_passing/queueing/thread.h"

namespace queueing {

nrn_thread_data::nrn_thread_data():
ite_received_(0), local_received_(0), enqueued_(0), delivered_(0) {
    input_parameters p;
    time_ = 0;
    char name[] = "coreneuron_1.0_queueing_data";
    std::string data = mapp::data_test();
    p.name = name;

    std::vector<char> chardata(data.begin(), data.end());
    chardata.push_back('\0');
    p.d = &chardata[0];
    nt_ = (NrnThread *) storage_get(p.name, make_nrnthread, p.d, free_nrnthread);
    if(nt_ == NULL){
        std::cerr<<"Error: Unable to open data file"<<std::endl;
        storage_clear(p.name);
        exit(EXIT_FAILURE);
    }
    inter_thread_events_.reserve(1000);
}

void nrn_thread_data::self_send(int d, double tt){
    ++enqueued_;
    ++local_received_;
    qe_.insert(tt, d);
}

void nrn_thread_data::inter_thread_send(int d, double tt){
    lock_.lock();
    event ite;
    ite.data_ = d;
    ite.t_ = tt;
    ++ite_received_;
    inter_thread_events_.push_back(ite);
    lock_.unlock();
}

void nrn_thread_data::inter_send_no_lock(int d, double tt){
    event ite;
    ite.data_ = d;
    ite.t_ = tt;
    inter_thread_events_.push_back(ite);
}

void nrn_thread_data::enqueue_my_events(){
    lock_.lock();
    event ite;
    for(int i = 0; i < inter_thread_events_.size(); ++i){
        ite = inter_thread_events_[i];
        ++enqueued_;
        qe_.insert(ite.t_, ite.data_);
    }
    inter_thread_events_.clear();
    lock_.unlock();
}

bool nrn_thread_data::deliver(){
    event q;
    if(qe_.atomic_dq(time_, q)){
        ++delivered_;

        // Use imitation of the point_receive of CoreNeron.
        // Varies per a specific simulation case.
        // Uses reduced version of net_receive of ProbAMPANMDA mechanism.
        mech_net_receive(nt_,&(nt_->ml[18]));
        return true;
    }
    return false;
}

void nrn_thread_data::l_algebra(){
    nt_->_t = static_cast<double>(time_);

       //Update the current
       mech_current_NaTs2_t(nt_,&(nt_->ml[17]));
       mech_current_Ih(nt_,&(nt_->ml[10]));
       mech_current_ProbAMPANMDA_EMS(nt_,&(nt_->ml[18]));

    //Call solver
    nrn_solve_minimal(nt_);

    //Update the states
    mech_state_NaTs2_t(nt_,&(nt_->ml[17]));
    mech_state_Ih(nt_,&(nt_->ml[10]));
    mech_state_ProbAMPANMDA_EMS(nt_,&(nt_->ml[18]));
}

} //endnamespace
