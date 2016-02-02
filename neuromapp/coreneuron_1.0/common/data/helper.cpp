/*
 * Neuromapp - helper.cpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/test/coreneuron_1.0/helper.cpp
 *  Helper for starting all the miniapp
 */

#include <cstdlib>
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "coreneuron_1.0/common/data/helper.h"

extern "C" {
#include "utils/storage/storage.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
}

namespace mapp{
    /** helper to execute the miniapp */
    int execute(std::vector<std::string> &v, int(*ptf)(int,char * const *)){
        std::vector<char*> command_vc;
        for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
	    command_vc.push_back((char*) it->c_str());
        return ptf(command_vc.size(), &command_vc[0]);
    }

    /** helper to compare to debug solution */
    void helper_check(std::string const& name, std::string const& mechanism, std::string const& path){
        //extract what we need d and rhs
        NrnThread * nt = (NrnThread *) storage_get (name.c_str(),
                                                    make_nrnthread, (void*)path.c_str(), free_nrnthread);

        int size = nt->end;
        double* rhs = nt->_actual_rhs; // compute solutiom rhs
        double* d = nt->_actual_d;  // compute solutiom d

        double* ref_rhs = new double[size];
        double* ref_d = new double[size];

        std::string data = mapp::data_ref() + "rhs_d_"+mechanism;

//        std::ofstream outfile(data);
//        for(int i=0; i < size;++i)
//            outfile << " "  << d[i] << " " <<  rhs[i] << "\n";

        std::ifstream infile(data.c_str(),std::ifstream::in);

        if(infile.good())
            for(int i=0; i < size;++i)
                infile >> ref_d[i] >> ref_rhs[i];

        infile.close();

        for(int i=0; i < size;++i){
            BOOST_CHECK_CLOSE( ref_d[i], d[i], 0.01 );
            BOOST_CHECK_CLOSE( ref_rhs[i], rhs[i], 0.01 );
        }

        delete [] ref_rhs;
        delete [] ref_d;
    }
}
