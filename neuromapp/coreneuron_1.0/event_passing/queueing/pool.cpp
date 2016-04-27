/*
 * Neuromapp - pool.cpp, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/event_passing/queueing/pool.cpp
 * \brief Contains pool class definition.
 */

#include "coreneuron_1.0/event_passing/queueing/pool.h"

namespace queueing {

pool::~pool(){
    int ite_received = 0;
    int local_received = 0;
    int all_enqueued = 0;
    int all_delivered = 0;
    for(int i=0; i < thread_datas_.size(); ++i){
        ite_received += thread_datas_[i].ite_received_;
        local_received += thread_datas_[i].local_received_;
        all_enqueued += thread_datas_[i].enqueued_;
        all_delivered += thread_datas_[i].delivered_;
        assert(thread_datas_[i].get_time() == time_);
    }

    if(rank_ == 0){
        std::cout<<"Total inter-thread received: "<<ite_received<<std::endl;
        std::cout<<"Total local received: "<<local_received<<std::endl;
        std::cout<<"Total spikes received: "<<received_<<std::endl;
    }

/*   std::cout<<"Total enqueued: "<<all_enqueued<<std::endl;
    std::cout<<"Total delivered: "<<all_delivered<<std::endl;
    std::cout<<"Total relevent spikes: "<<relevant_<<std::endl;
    neuromapp_data.put_copy("inter_received", ite_received);
    neuromapp_data.put_copy("local_received", ite_received);
    neuromapp_data.put_copy("enqueued", all_enqueued);
    neuromapp_data.put_copy("delivered", all_delivered);*/
}

} //end of namespace
