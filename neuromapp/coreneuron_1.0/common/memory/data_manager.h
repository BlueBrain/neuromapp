/*
* Neuromapp - data_manager.h, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* paramod.kumbhar@epfl.ch
* All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3.0 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.
*/

/**
 * @file neuromapp/coreneuron_1.0/common/memory/data_manager.h
 * Implements openacc technology for coreneuron1.0
 */

#ifndef MAPP_DATA_MANAGER_
#define MAPP_DATA_MANAGER_

/** \fn void setup_nrnthreads_on_device(NrnThread *nt)
    \brief allocate and transfert data on device using openacc technology
    \param NrnThread the data structure to transfer
*/
void setup_nrnthreads_on_device(NrnThread *nt);

/** \fn void update_nrnthreads_on_host(NrnThread *nt)
    \brief update the memory on the host
    \param NrnThread the data structure to update
*/
void update_nrnthreads_on_host(NrnThread *nt);


#endif
