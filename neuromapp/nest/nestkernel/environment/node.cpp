/*
 * node.cpp
 *
 *  Created on: May 2, 2016
 *      Author: schumann
 */


#include "nest/nestkernel/environment//node.h"

void nest::spikedetector::handle( nest::spikeevent& e )
{
    spikes.push_back(e);
}

void nest::spikecounter::handle( nest::spikeevent& e )
{
    num += 1;
    sumtime += e.get_stamp().get_ms();
}
