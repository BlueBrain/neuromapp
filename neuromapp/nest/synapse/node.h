/*
 * Neuromapp - event.h, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * till.schumann@epfl.ch,
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
 * @file neuromapp/nest/synapse/event.h
 * \brief Node class
 */


#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include "nest/synapse/event.h"




namespace nest
{

class spikeevent;

typedef size_t index;
typedef int thread;

typedef void Network;
typedef void Subnet;


    /**
     * \struct node
     * \brief nest node
     */
    class node
    {
    private:
        index gid_;          //!< Global element id (within network).
        index lid_;          //!< Local element id (within parent).
        index subnet_index_; //!< Index of node in parent's node array

        /**
         * Local id of this node in the thread-local vector of nodes.
         */
        index thread_lid_;

        /**
         * Model ID.
         * It is only set for actual node instances, not for instances of class Node
         * representing model prototypes. Model prototypes always have model_id_==-1.
         * @see get_model_id(), set_model_id()
         */
        int model_id_;
        Subnet* parent_;           //!< Pointer to parent.
        thread thread_;            //!< thread node is assigned to
        thread vp_;                //!< virtual process node is assigned to
        bool frozen_;              //!< node shall not be updated if true
        bool buffers_initialized_; //!< Buffers have been initialized
        bool needs_prelim_up_;     //!< node requires preliminary update step


    protected:
        static Network* net_; //!< Pointer to global network driver.
    public:
        virtual void handle( spikeevent& e ) = 0;

                inline void set_lid(short lid) { lid_ = lid; }

                inline short set_lid() const { return lid_; }
    };

    class spikedetector : public node
    {
    public:
        std::vector<double> spikes;
        void handle( spikeevent& e );
    };
};

#endif
