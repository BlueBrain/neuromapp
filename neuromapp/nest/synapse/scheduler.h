/*
 * Neuromapp - event.h, Copyright (c), 2015,
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
 * @file neuromapp/nest/synapse/scheduler.h
 * \brief  Event classes to handle spike events
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>
#include <cassert>
#include <iostream>
#include "nest/synapse/node.h"
#include "nest/synapse/connector_base.h"
#include "nest/libnestutil/sparsetable.h"

namespace nest
{
    class ConnectorBase; //forward declaration
    class tsodyks2;

    class node; //forward declaration
    
    typedef unsigned int targetindex;

    class scheduler
    {
    private:
        static std::vector<node*> nodes_vec_;

    public:
        scheduler()
        {
            nodes_vec_.clear();
        }
        
        ~scheduler()
        {
            nodes_vec_.clear();
        }
        
        /**
         * \fn update_nodes_vec(std::vector<node*>& nodes)
         * \brief loads nodes_vec with an existing vector
         */
        inline static void update_nodes_vec(std::vector<node*>& nodes)
        {
            nodes_vec_ = nodes;
        }

        /**
         * \fn add_node(node*)
         * \brief adds a new node to the node_vec_
         */
        inline static targetindex add_node(node* n)
        {
            nodes_vec_.push_back(n);
	    
	    //
    	    assert(nodes_vec_.size() < std::numeric_limits<targetindex>::max() );
	    
            return static_cast<targetindex>(nodes_vec_.size()-1);
        }

        /**
         * \fn get_target(short index)
         * \brief getter to retrieve node from nodes_vec_
         */
        inline static node* get_target(targetindex index) //corresponds to scheduler::thread_lid_to_node
        {
            assert(index >= 0 && index < nodes_vec_.size());
            return nodes_vec_[index]; //in NEST the vector contains all reference for all threads.
        }

    };

}; // namespace nest
#endif
