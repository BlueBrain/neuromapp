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
    value_type accumulate_reciprocal(0.);
    std::for_each(v.begin(), v.end(),std::bind1st(std::divides<value_type>(),1.)); // v[i] -> 1/v[i], for fun
    accumulate_reciprocal = keyvalue::utils::accumulate(v.begin(), v.end(), 0.); // MPI is inside
    g_iops = accumulate_reciprocal*a.cg();
    g_mbw = accumulate_reciprocal*a.voltages_size() * sizeof(value_type)/(1024.*1024.);
}

void statistic::print(std::ostream& os) const{
    std::vector<value_type>::const_iterator it = v.begin();
    while(it != v.end()){
    std::cout << "  Time: " << *it << "[s] \n"
              << "  I/O: "  << a.cg()/(*it) << " IOPS \n"
              << "  BW: "   << a.voltages_size()* sizeof(double)/((*it)*1024.*1024.) << " MB/s \n";
                ++it;
    }
    os << " g_iops " << g_iops << " [IOPS], " << " g_mbw "  << g_mbw << " [MB/s] \n";
}

}