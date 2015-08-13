

/*
 * Neuromapp - driver.cpp, Copyright (c), 2015,
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
 * @file neuromapp/app/driver.cpp
 * Driver implementation
 */

#include "app/driver.h"

namespace mapp{

    void usage(){
        std::cout << "Usage: < >  means optional \n";
        std::cout << "   miniapp --help provides the help of your miniapp \n \n";
        std::cout << "   The list of the following miniapps are available: \n";
        std::cout << "       hello <arg> \n";
        std::cout << "       kernel <arg> \n";
        std::cout << "       solver <arg> \n";
        std::cout << "       fullComputationStep <arg> \n";
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
