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
 * \brief  Event classes to handle spike events
 */


#ifndef EVENT_H_
#define EVENT_H_

#include <vector>
#include <cstddef>
#include "nest/synapse/node.h"
#include <limits.h>
#include <limits>

namespace nest
{
struct Time {
    long tics;
    Time(): tics(0) {}
    Time(const double& t): tics(t*1000.0) {}
     double get_ms() const
     {
        if ( tics == LONG_MAX / (long)8. + 1 ) {
          return std::numeric_limits<double>::max();
        }
        if ( tics == -LONG_MAX / (long)8 + 1 ) {
          return -std::numeric_limits<double>::max();
        }
        return 1/1000.0 * tics;
     }
};

typedef size_t index;
typedef long port;
typedef long rport;

class node;


    /**
     * \struct event
     * \brief It represents a spike event copied from NEST 2.10
     */
    struct event
    {
          index sender_gid_; //!< GID of sender or -1.
                             /*
                              * The original formulation used references to Nodes as
                              * members, however, in order to avoid the reference of reference
                              * problem, we store sender and receiver as pointers and use
                              * references in the interface.
                              * Thus, we can still ensure that the pointers are never NULL.
                              */
          node* sender_;     //!< Pointer to sender or NULL.
          node* receiver_;   //!< Pointer to receiver or NULL.


          /**
           * Sender port number.
           * The sender port is used as a unique identifier for the
           * connection.  The receiver of an event can use the port number
           * to obtain data from the sender.  The sender uses this number to
           * locate target-specific information.  @note A negative port
           * number indicates an unknown port.
           */
          port p_;

          /**
           * Receiver port number (r-port).
           * The receiver port (r-port) can be used by the receiving Node to
           * distinguish incoming connections. E.g. the r-port number can be
           * used by Events to access specific parts of a Node. In most
           * cases, however, this port will no be used.
           * @note The use of this port number is optional.
           * @note An r-port number of 0 indicates that the port is not used.
           */
          rport rp_;

          /**
           * Transmission delay.
           * Number of simulations steps that pass before the event is
           * delivered at the receiver.
           * The delay must be at least 1.
           */
          long d_;

          /**
           * Time stamp.
           * The time stamp specifies the absolute time
           * when the event shall arrive at the target.
           */
          Time stamp_;

          /**
           * Offset for precise spike times.
           * offset_ specifies a correction to the creation time.
           * If the resolution of stamp is not sufficiently precise,
           * this attribute can be used to correct the creation time.
           * offset_ has to be in [0, h).
           */
          double offset_;

          /**
           * Weight of the connection.
           */
          double w_;

          /**
         * Deliver the event to receiver.
         *
         * This operator calls the handler for the specific event type at
         * the receiver.
         */
         virtual void operator()() = 0;

         const double get_weight() const
         {
             return w_;
         }

         void set_stamp( Time const& stamp )
         {
             stamp_ = stamp;
         }

         const Time& get_stamp() const
         {
             return stamp_;
         }

         void set_receiver( node* node ) // ref in NEST
         {
             receiver_ = node;
         }
         void set_sender( node* node ) // ref in NEST
         {
             sender_ = node;
         }

         void set_weight(double w)
         {
             w_ = w;
         }

         void set_delay( long d )
         {
             d_ = d;
         }

         /**
          * Return transmission delay of the event.
          * The delay refers to the time until the event is
          * expected to arrive at the receiver.
          */
         inline long get_delay() const
         {
             return d_;
         }
    };
    /**
     * \class spikeevent
     * \brief Event for spike information. Used to send a spike from one node to the next.
     */
    class spikeevent : public event
    {
    public:
        //spikeevent();
        void operator()();
        //SpikeEvent* clone() const;

        //void set_multiplicity( int_t );
        //int_t get_multiplicity() const;
        inline spikeevent()
                  : multiplicity_( 1 )
                {}
        protected:
                int multiplicity_;

    };
};
#endif /* EVENT_H_ */
