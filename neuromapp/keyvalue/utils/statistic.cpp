/*
 * Neuromapp - statistic.cpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/keyvalue/utils/statistic.cpp
 * \brief basic shell for arguments
 */

#include <numeric>
#include <algorithm>

#include "keyvalue/utils/statistic.h"
#include "keyvalue/utils/tools.h"

namespace keyvalue {

void statistic::process(){
    // Check the type of computation: task dependency or loop
    if (a.taskdeps()) {
        // Base time without I/O (v[0])
        value_type tbase = v[0];
        // Regular time with I/O (v[1])
        value_type tio = v[1];

        // IOPS and BW are computed in the following way:
        // 1. Compute IOPS and BW per rank
        double time = tio - tbase;
        double r_iops = (it_ops) / time;
        double r_mbw = (op_size * it_ops) / time;
        // 2. Compute the mean IOPS and BW for all ranks
        g_iops = keyvalue::reduce(r_iops);
        g_mbw = keyvalue::reduce(r_mbw);
    } else {
        // IOPS and BW are computed in the following way:
        // 1. Accumulate the time of all iterations per rank
        double time = std::accumulate(v.begin(), v.end(), 0.);
        // 2. Compute IOPS and BW per rank
        double r_iops = (it_ops) / time;
        double r_mbw = (op_size * it_ops) / time;
        // 3. Compute the mean IOPS and BW for all ranks
        g_iops = keyvalue::reduce(r_iops);
        g_mbw = keyvalue::reduce(r_mbw);
    }
}

void statistic::print(std::ostream& os) const{
    os << "Mini-app configuration:" << std::endl;
    a.print(os);

    if (a.taskdeps()) {
        os << "Computation time reference without I/O: " << v[0] << " s." << std::endl
                << "Computation time with I/O: " << v[1] << " s." << std::endl;
    } //else {
        //unsigned int iter = 0;
        //std::vector<value_type>::const_iterator it = v.begin();
        //while(it != v.end()){
            //os << "[iter " << iter << "]\t" << *it << " s,\t" << a.cg()/(*it) << " IOPS,\t"
            //        << a.voltages_size()* sizeof(double)/((*it)*1024.*1024.) << " MB/s" << std::endl;
            //++it;
            //iter++;
        //}
    //}
    os << "I/O: " << g_iops << " IOPS" << std::endl
            << "BW: "  << g_mbw << " MB/s" << std::endl;

    // CSV output data format:
    // miniapp_name, num_procs, num_threads/proc, implementation,
    // usecase, simtime (ms), mindelay (ms), dt (ms), cell_groups,
    // backend, sync/async, flash/disk, iops (kIOP/S), bw (GB/s)
    os << "KVMAPP," << a.procs() << "," << a.threads() << "," << ( a.taskdeps() ? "deps" : "loop" ) << ","
            << a.usecase() << "," << a.st() << "," << a.md() << "," << a.dt() << "," << a.cg() << ","
            << a.backend() << "," << ( a.async() ? "async" : "sync" ) << ","
            << ( a.flash() ? "flash" : "disk" ) << "," << std::fixed << g_iops << "," << g_mbw << std::endl;
}

}
