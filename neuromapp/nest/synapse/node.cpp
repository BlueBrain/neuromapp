/*
 * node.cpp
 *
 *  Created on: May 2, 2016
 *      Author: schumann
 */


#include "nest/synapse/node.h"

void nest::spikedetector::handle( nest::spikeevent& e )
{
	spikes.push_back(e.get_stamp().get_ms());
}
