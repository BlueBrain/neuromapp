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

namespace nest{
	class Tsodyks2
	{
	public:
		Tsodyks2(const double& delay,
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

		void send(Event& e, double t_lastspike) {
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

		double get_delay() const
		{
			return delay_;
		}

		void set_delay(double delay) {
			delay_ = delay;
		}

		double get_tau_fac() const {
			return tau_fac_;
		}

		void set_tau_fac(double tauFac) {
			tau_fac_ = tauFac;
		}

		double get_tau_rec() const {
			return tau_rec_;
		}

		void set_tau_rec(double tauRec) {
			tau_rec_ = tauRec;
		}

		double get_u() const {
			return u_;
		}

		void set_u(double u) {
			u_ = u;
		}

		double get_U() const {
			return U_;
		}

		void set_U(double u) {
			U_ = u;
		}

		double get_weight() const {
			return weight_;
		}

		void set_weight(double weight) {
			weight_ = weight;
		}

		double get_x() const {
			return x_;
		}

		void set_x(double x) {
			x_ = x;
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
