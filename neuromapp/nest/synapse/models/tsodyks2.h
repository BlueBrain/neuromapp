/*
 * Neuromapp - tsodyks2.h, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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

#include "nest/synapse/event.h"

namespace nest
{
	/**
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
	class tsodyks2
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
		tsodyks2(const double& delay,
				 const double& weight,
				 const double& U,
				 const double& u,
				 const double& x,
				 const double& tau_rec,
				 const double& tau_fac) :
			delay_(delay),
			weight_(weight),
			U_(U),
			u_(u),
			x_(x),
			tau_rec_(tau_rec),
			tau_fac_(tau_fac)
		{}

		/** \fn void send()
			        \brief Sends a spike event through the synapse
			        \param e spike event
    				\param t_lastspike time of last spike
			     */
		void send(event& e, double t_lastspike)
		{
			double h = e.t - t_lastspike;
			double x_decay = std::exp(-h / tau_rec_);
			double u_decay = (tau_fac_ < 1.0e-10) ? 0.0 : std::exp(-h / tau_fac_);
			// now we compute spike number n+1
			x_ = 1. + (x_ - x_ * u_ - 1.) * x_decay; // Eq. 5 from reference [3]
			u_ = U_ + u_ * (1. - U_) * u_decay; // Eq. 4 from [3]
			e.weight = x_ * u_ * weight_;
			e.delay = delay_;
			e();
		}

		/** \fun delay() const
		        \brief get delay, read only */
		const double& delay() const
		{
			return delay_;
		}

		/** \fun delay()
				\brief returns a reference to delay */
		double& delay()
		{
			return delay_;
		}

		/** \fun tau_fac() const
				        \brief get tau_fac, read only */
		const double& tau_fac() const
		{
			return tau_fac_;
		}

		/** \fun tau_fac()
						\brief returns a reference to tau_fac */
		double& tau_fac()
		{
			return tau_fac_;
		}

		/** \fun tau_rec() const
				        \brief get tau_rec, read only */
		const double& tau_rec() const
		{
			return tau_rec_;
		}

		/** \fun tau_rec()
						\brief returns a reference to tau_rec */
		double& tau_rec()
		{
			return tau_rec_;
		}

		/** \fun u() const
				        \brief get u, read only */
		const double& u() const
		{
			return u_;
		}

		/** \fun u()
						\brief returns a reference to u */
		double& u()
		{
			return u_;
		}

		/** \fun U() const
				        \brief get U, read only */
		const double& U() const
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
		const double& weight() const
		{
			return weight_;
		}

		/** \fun weight()
								\brief returns a reference to weight */
		double& weight()
		{
			return weight_;
		}

		/** \fun x() const
						        \brief get x, read only */
		const double& x() const
		{
			return x_;
		}

		/** \fun x()
										\brief returns a reference to x */
		double& x()
		{
			return x_;
		}

	private:
		double delay_;  //!< synapse weight
		double weight_; //!< synapse weight
		double U_; //!< unit increment of a facilitating synapse
		double u_; //!< dynamic value of probability of release
		double x_; //!< current fraction of the synaptic weight
		double tau_rec_; //!< [ms] time constant for recovery
		double tau_fac_; //!< [ms] time constant for facilitation
	};
};
#endif /* TSODYKS2_H_ */
