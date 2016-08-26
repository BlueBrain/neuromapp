

/*
 * Neuromapp - driver.cpp, Copyright (c), 2015,
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
 * @file neuromapp/app/driver.cpp
 * \brief Driver implementation
 */
#include <string.h>
#include <set>
#include <iterator>

#include "app/driver.h"
#include "coreneuron_1.0/common/data/helper.h"

namespace mapp{

    void driver::usage() const{
        std::string text = "Usage: < >  means optional \n";
        text += "   miniapp --help provides the help of your miniapp \n \n";
        text += "   The list of the following miniapps are available:\n";
        for(std::map<std::string,int(*)(int,char * const *)>::const_iterator it = m.begin(); it != m.end(); ++it)
           text += ("      "+ it->first + " <arg> \n"); //extract all kernel in the driver, naturaly sort
        text += "   quit to exit \n";
        text += "   The miniapp: kernel, solver, cstep can use the provided data set: \n";
        text +=  "\n";
        std::cout << text + "       "+mapp::data_test()+" \n";
    }

    void driver::insert(const std::string& name, int(*f)(int,char *const *) ){
        m.insert(make_pair(name,f));
    }

    void driver::execute(int argc, char * const argv[]) const{
        if(argc == 1)
            usage();
        else{
            std::map<std::string, int(*)(int,char *const *)>::const_iterator it;
            it = m.find(std::string(argv[1]));
            if(it == m.end())
                usage();
            else{
                int error = it->second(argc-1, &argv[1]);
                if(error)
                    throw driver_exception(error);
            }
        }
    }
}// end namespace
