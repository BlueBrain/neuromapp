
/*
 * Neuromapp - driver.h, Copyright (c), 2015,
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
 * @file neuromapp/app/driver.h
 * Driver implementation
 */

#ifndef MAPP_DRIVER_
#define MAPP_DRIVER_

#include <iostream>
#include <string>
#include <map>

#include "app/driver_exception.h"


//! Generic namespace for all the miniapps
namespace mapp{

    /** \fn int usage()
        \brief Print the usage of the driver
     */
    void usage();

    /** \class driver for the execution of the miniapps

    \brief The driver encapsulates functional of all miniapp.

    the key is the name of the miniapp, the use should provide the a function, XXXX_execute function respecting
    the tiny API.

        int XXXX_execute(int argc, char* argv);

    The user is responsible of the helper
    \remark the name is optional nevertheless all mini-apps must have differents name for the execution
    function, we suggest NAMEMINIAPP_execute(int argc, char* argv)
    */
    class driver{
        public:

        /** \param name key for the functor of the miniapp
            \param f functor the miniapp
         */
        void insert(const std::string name, int(*f)(int,char *const *) );
        /**  \brief find the corresponding functor using the key provide by the user (command line).
                    if it fails an exception is generated
             \param argc number of argument in the command line
             \param argv the commane line
         */
        void execute(int argc, char * const argv[]);

        private:
        /** Map containing functor of miniapp assocaited to key */
        std::map<std::string, int(*)(int,char * const *) > m;
    };

}// end namespace

#endif
