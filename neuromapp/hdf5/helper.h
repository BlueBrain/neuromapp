/*
 * Neuromapp - helper.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
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
 * @file neuromapp/hdf5/data/helper.h
 *  Helper for starting all the miniapp
 */

#ifndef MAPP_HDF5_DATA_HELPER_H_
#define MAPP_HDF5_DATA_HELPER_H_

#include <string.h>

#include "neuromapp/hdf5/data/path.h" // this file is generated automatically

namespace h5mapp{
    /** helper for the path of the input data */
    inline std::string hdf5_testdata_datasets(){
        return h5mapp::helper_build_path::test_data_path()+"syn_test_126.h5";
    }

    /** helper for the path of the input data */
    inline std::string hdf5_testdata_compound(){
        return h5mapp::hdf5::helper_build_path::test_data_path()+"syn_test_126_comp.h5";
    }
}

#endif
