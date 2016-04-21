/*
 * Neuromapp - event.h, Copyright (c), 2015,
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
 * @file neuromapp/nest/synapse/event.h
 * \brief  Event classes to handle spike events
 */


#ifndef EVENT_H_
#define EVENT_H_

#include <vector>

namespace nest{

/**
 * Spike event class
 */
class Event
{
public:

	double t;
	int sender;
	int receiver;

	double weight;
	double delay;

	virtual void operator()() {}
};

/**
 * Spike event class.
 * The Event class is extended with a weight logging vector.
 */
class VectorEvent: public Event
{
private:
	std::vector<double>& ptr_weights;
public:
	VectorEvent(const double& t,
			    const int& sender,
			    const int& receiver,
			    const double& weight,
			    const double& delay,
			    std::vector<double>& ptr_weights):
				   ptr_weights(ptr_weights)
	{
		   this->t = t;
		   this->sender = sender;
		   this->receiver = receiver;
		   this->weight = weight;
		   this->delay = delay;
	}
	void operator()()
	{
		ptr_weights.push_back(weight);
	}
};

};

#endif /* EVENT_H_ */
