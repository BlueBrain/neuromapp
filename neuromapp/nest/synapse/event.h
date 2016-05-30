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

namespace nest
{
	/**
	 * \struct event
	 * \brief It represents a spike event
	 */
	struct event
	{
	    double t;
       	    int sender;
	    int receiver;
	    
	    double weight;
	    double delay;

       /** \fn virtual void operator()() 
        *\brief has to be implemented by inherited class
       */
            virtual void operator()(){}
	};
	/**
	 * \class logevent
	 * \brief The event class is extended with a weight logging vector.
	 */
	class logevent: public event
	{
	private:
  	    std::vector<double>& weightlog;
	public:
	/** \fn logevent(const double& t, const int& sender, const int& receiver, 
	                 const double& weight, const double& delay, std::vector<double>& weightlog)
        \brief Constructor of the logevent class
        \param t time of event
	\param sender event sender
	\param receiver event receiver
	\param weight weight
	\param delay delay
	\param weightlog reference to the weight log vector
	*/
	    logevent(const double& t,
	     	     const int& sender,
	             const int& receiver,
	             const double& weight,
	             const double& delay,
	             std::vector<double>& weightlog):
	             weightlog(weightlog){
	             	this->t = t;
	             	this->sender = sender;
	             	this->receiver = receiver;
	             	this->weight = weight;
	             	this->delay = delay;
	             	
	             }
        /** \fn void operator()()
        \brief Pushes the current weight to the log vector
        */
        void operator()(){
        	    weightlog.push_back(weight);
        }
    };
};// end namespace
#endif /* EVENT_H_ */
