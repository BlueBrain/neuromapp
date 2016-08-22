

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

    void usage(){
        std::string text = "Usage: < >  means optional \n";
        text += "   miniapp --help provides the help of your miniapp \n \n";
        text += "   The list of the following miniapps are available: \n";
        std::cout << text;
        std::set<std::string> s; // print in alphabetical order
        s.insert("       hello <arg> \n");
        s.insert("       synapse <arg> \n");
        s.insert("       event <arg> \n");
        s.insert("       kernel <arg> \n");
        s.insert("       solver <arg> \n");
        s.insert("       cstep <arg> \n");
        s.insert("       keyvalue <arg> \n");
        s.insert("       replib <arg> \n");
        std::copy(s.begin(),s.end(),std::ostream_iterator<std::string>(std::cout," "));
        text = "   quit to exit \n";
        text += "   The miniapp: kernel, solver, cstep can use the provided data set: \n";
        text +=  "\n";
        std::cout << text + "       "+mapp::data_test()+" \n";
    }

    void driver::insert(const std::string name, int(*f)(int,char *const *) ){
        m.insert(std::pair<std::string, int(*)(int,char *const *)>(name,f));
    }

    void driver::execute(int argc, char * const argv[]){
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

