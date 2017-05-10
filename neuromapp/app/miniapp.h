/*
 * Neuromapp - miniapp.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/app/miniapp.h
 * \brief File containing the list of enabled mini-apps to register in the driver
 */

#ifndef MAPP_APP_
#define MAPP_APP_


#if NEUROMAPP_HELLO_MAPP
#include "hello/hello.h"
#endif

#if NEUROMAPP_NEST_MAPP
#include "nest/drivers/synapse.h"
#include "nest/drivers/h5import.h"
#endif

#if NEUROMAPP_HDF5_MAPP
#include "hdf5/drivers/h5read.h"
#endif

//#if NEUROMAPP_CORENEURON_MAPP
#include "coreneuron_1.0/event_passing/drivers/drivers.h"
#include "coreneuron_1.0/kernel/kernel.h"
#include "coreneuron_1.0/solver/solver.h"
#include "coreneuron_1.0/cstep/cstep.h"
#include "coreneuron_1.0/queue/queue.h"
//#endif

#if NEUROMAPP_KEYVALUE_MAPP
#include "keyvalue/keyvalue.h"
#endif

#if NEUROMAPP_REPLIB_MAPP
#include "replib/replib.h"
#endif

#if NEUROMAPP_IOBENCH_MAPP
#include "iobench/iobench.h"
#endif

#include "app/driver.h"

//! Generic namespace for all the mini-apps
namespace mapp{

void register_miniapps(mapp::driver &d) {

#if NEUROMAPP_HELLO_MAPP
    d.insert("hello",hello_execute);
#endif
#if NEUROMAPP_NEST_MAPP
    d.insert("synapse", nest::model_execute);
    d.insert("nest_h5import", nest::h5import::execute);
#endif
#if NEUROMAPP_HDF5_MAPP
    d.insert("h5read", hdf5::h5read::execute);
#endif
//#if NEUROMAPP_CORENEURON_MAPP
    d.insert("event",event_execute);
    d.insert("kernel",coreneuron10_kernel_execute);
    d.insert("solver",coreneuron10_solver_execute);
    d.insert("cstep",coreneuron10_cstep_execute);
    d.insert("queue",coreneuron10_queue_execute);
//#endif
#if NEUROMAPP_KEYVALUE_MAPP
    d.insert("keyvalue",keyvalue_execute);
#endif
#if NEUROMAPP_REPLIB_MAPP
    d.insert("replib",replib_execute);
#endif
#if NEUROMAPP_IOBENCH_MAPP
    d.insert("iobench",iobench_execute);
#endif

}

} // namespace mapp

#endif
