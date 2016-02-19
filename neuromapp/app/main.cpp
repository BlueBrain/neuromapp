/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
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
 * @file neuromapp/app/main.cpp
 * \brief the main program
 */

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <vector>
#include <algorithm>

#ifdef NEUROMAPP_CURSOR
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

#include "app/miniapp.h" // the list of the miniapp API
#include "app/driver.h"
#include "app/driver_exception.h"
#include "utils/argv_data.h"



/** \brief the main program interacting with the user. The program is command line interactive. */
int main(int argc, char * const argv[]){

     mapp::driver d;
     d.insert("hello",hello_execute);
     d.insert("queueing",queueing_execute);
     d.insert("spike",spike_execute);
     d.insert("kernel",coreneuron10_kernel_execute);
     d.insert("solver",coreneuron10_solver_execute);
     d.insert("cstep",coreneuron10_cstep_execute);
     d.insert("keyvalue",keyvalue_execute);

     //direct run
     if(argv[1] != NULL){
         try {
             d.execute(argc,argv);
         } catch(mapp::driver_exception & e) {
             if(e.error_code != mapp::MAPP_USAGE)
                 std::cerr << "caught exception: " << e.what() << "\n";
         } catch(std::exception & e) {
                 std::cerr << "caught exception: " << e.what() << "\n";
         }
         return 0;
     }

     std::cout << "Welcome to NeuroMapp! Please enter "
               << "the name of the miniapp you wish to execute "
               << "followed by any arguments you wish to pass to it."
               << std::endl
               << "Please avoid preceding and trailing whitespace!"
               << std::endl
               << "To finish type quit"
               << std::endl
               << ">? ";

     // interactive run
     while(1) {
#ifdef NEUROMAPP_CURSOR
         input = readline("");
         add_history(input);
         std::string command(input);
#else
         std::string command;
         std::getline(std::cin, command);
#endif
         // I need to split the string into an array of strings to pass it
         // in an argv style
         std::vector<std::string> command_v;
         command_v.push_back(argv[0]);
         std::istringstream command_stream(command);
         std::istream_iterator<std::string> wb(command_stream),we;
         std::copy(wb,we,std::back_inserter(command_v));

         if( command_v[1].compare("quit") == 0 ) break;

         mapp::argv_data A(command_v.begin(),command_v.end());

         try {
             d.execute(A.argc(),A.argv());
         } catch(mapp::driver_exception & e) {
             if(e.error_code != mapp::MAPP_USAGE)
                 std::cerr << "caught exception: " << e.what() << "\n";
         } catch(std::exception & e) {
             std::cerr << "caught exception: " << e.what() << "\n";
         }
         std::cout << std::endl << ">? ";
     }

     return 0;
}


