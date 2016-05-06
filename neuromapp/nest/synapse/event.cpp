/*
 * event.cpp
 *
 *  Created on: May 2, 2016
 *      Author: schumann
 */

#include "nest/synapse/event.h"

void nest::spikeevent::operator()()
{
	  receiver_->handle( *this );
}
