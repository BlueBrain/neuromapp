/*
 * Neuromapp - tsodyks2.h, Copyright (c), 2015,
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
 * @file neuromapp/nest/synapse/models/tsdodyks2.h
 * \brief  Tsdodyks2 synapse model
 */

#ifndef TSODYKS2_H_
#define TSODYKS2_H_

#include <boost/program_options.hpp>

#include "nest/synapse/scheduler.h"
#include "nest/synapse/event.h"

namespace nest
{

typedef double weight;

struct connection{
    connection():target_(-1),delay_(2){}
    targetindex target_; //simplification of hpc synapses from NEST
    long delay_; //!< syn_id (char) and delay (24 bit) in timesteps of this connection - stored differently in NEST
};


    /**
     *
     * \class tsodyks2
     * \brief tsodyks2 model from NEST
     *
     * NEST synapse model: Tsodyks2
     *  * This synapse model implements synaptic short-term depression and short-term facilitation
     * according to [1] and [2]. It solves Eq (2) from [1] and modulates U according to eq. (2) of [2].
     * [1] Tsodyks, M. V., & Markram, H. (1997). The neural code between neocortical pyramidal neurons
     *  depends on neurotransmitter release probability. PNAS, 94(2), 719-23.
     * [2] Fuhrmann, G., Segev, I., Markram, H., & Tsodyks, M. V. (2002). Coding of temporal
     * information by activity-dependent synapses. Journal of neurophysiology, 87(1), 140-8.
     * [3] Maass, W., & Markram, H. (2002). Synapses as dynamic memory buffers. Neural networks, 15(2),
     * 155–61.synapse/args
     */
    class tsodyks2 : public connection /// 10k of this synapse per neuron
    {
    public:
        /** \fun Tsodyks2(const double& delay, const double& weight, const double& U, const double& u, const double& x, const double& tau_rec, const double& tau_fac)
                \brief Constructor of the Tsodyks2 class
                \param delay delay
                \param weight weight
                \param U U
                \param u u
                \param x x
                \param tau_rec tau_rec
                \param tau_fac tau_fac
                */
        tsodyks2(const long& delay = 2,
                 const double& w = 1.0,
                 const double& U = 0.5,
                 const double& u = 0.5,
                 const double& x = 1.0,
                 const double& tau_rec = 800.0,
                 const double& tau_fac = 0.0,
                 const targetindex target=-1) :
            weight_(w),
            U_(U),
            u_(u),
            x_(x),
            tau_rec_(tau_rec),
            tau_fac_(tau_fac)
        {
            delay_ = delay;
            target_=target;

            #ifdef _DEBUG //model parameters are only checked in debug mode
            if ( U_ > 1.0 || U_ < 0.0 ) {
                throw std::invalid_argument( "U must be in [0,1]." );
            }
            if ( u_ > 1.0 || u_ < 0.0 ) {
                hrow std::invalid_argument( "u must be in [0,1]." );
            }
            if ( tau_rec_ <= 0.0 ) {
                throw std::invalid_argument( "tau_rec must be > 0." );
            }
            if ( tau_fac_ < 0.0 ) {
                throw std::invalid_argument( "tau_fac must be >= 0." );
            }
            #endif //_DEBUG
        }

        /** \fn void send()
                \brief Sends a spike event through the synapse as implemented in NEST software 2.10 (2016) official release
                    \brief In nest we execute this function once per synapse per time step (worst case)
                    \brief On average --> to be added to use case document
                    \param e spike event
                    \param t_lastspike time of last spike
                 */
        inline void send(event& e, double t_lastspike)
        {
            double h = e.get_stamp().get_ms() - t_lastspike;
            double x_decay = std::exp(-h / tau_rec_); /// To be checked which implementation of exponential is being used
            double u_decay = (tau_fac_ < 1.0e-10) ? 0.0 : std::exp(-h / tau_fac_); // branching
            // now we compute spike number n+1
            /// no forward dependency between next 2 statements
            x_ = 1. + (x_ - x_ * u_ - 1.) * x_decay; // Eq. 5 from reference [3] ---> 2 Multiply + 3 adds + 1 assignment
            u_ = U_ + u_ * (1. - U_) * u_decay; // Eq. 4 from [3] --> 2 Muliply + 2 adds + 1 assignment
            node* target_node = scheduler::get_target(target_); //reduced call tree in comparison to NEST. Further, thread number is passed to get_target
            assert(target_node != NULL);
            e.set_receiver( target_node ); //simplification
            e.set_weight(x_ * u_ * weight_); // weight constant for the object after the synapase is created (can we use const?) --> 2 Multiply +  1 assignment
            //e.set_delay( delay_ ); //  1 assignment
            //e.set_rport( -1 );
            e(); // append right now, in nest sending to post synaptic neuron
        }
        /** \fun delay() const
            \brief get delay, read only */
        inline const long& delay() const
        {
            return delay_;
        }

        /** \fun delay()
            \brief returns a reference to delay */
        inline long& delay()
        {
            return delay_;
        }

        /** \fun tau_fac() const
            \brief get tau_fac, read only */
        inline const double& tau_fac() const
        {
            return tau_fac_;
        }

        /** \fun tau_fac()
            \brief returns a reference to tau_fac */
        inline double& tau_fac()
        {
            return tau_fac_;
        }

        /** \fun tau_rec() const
            \brief get tau_rec, read only */
        inline const double& tau_rec() const
        {
            return tau_rec_;
        }

        /** \fun tau_rec()
            \brief returns a reference to tau_rec */
        inline double& tau_rec()
        {
            return tau_rec_;
        }

        /** \fun u() const
            \brief get u, read only */
        inline const double& u() const
        {
            return u_;
        }

        /** \fun u()
            \brief returns a reference to u */
        inline double& u()
        {
            return u_;
        }

        /** \fun U() const
            \brief get U, read only */
        inline const double& U() const
        {
            return U_;
        }

        /** \fun U()
            \brief returns a reference to U */
        double& U()
        {
            return U_;
        }

        /** \fun weight() const
            \brief get weight, read only */
        inline const double& weight() const
        {
            return weight_;
        }

        /** \fun weight()
            \brief returns a reference to weight */
        inline double& weight()
        {
            return weight_;
        }

        /** \fun x() const
        \brief get x, read only */
        inline const double& x() const
        {
            return x_;
        }

        /** \fn x()
         *  \brief returns a reference to x */
        inline double& x()
        {
            return x_;
        }

    private:
        double weight_; //!< synapse weight
        double U_; //!< unit increment of a facilitating synapse
        double u_; //!< dynamic value of probability of release
        double x_; //!< current fraction of the synaptic weight
        double tau_rec_; //!< [ms] time constant for recovery
        double tau_fac_; //!< [ms] time constant for facilitation
    };
};
#endif /* TSODYKS2_H_ */
