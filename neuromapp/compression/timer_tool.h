/*
 * Neuromapp - timer_tool.h, Copyright (c), 2015,
 * Devin Bayly - University of Arizona
 * baylyd@email.arizona.edu,
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
#ifndef TIMER_INCLUDE_H
#define TIMER_INCLUDE_H
#include <chrono>
#include <string>
#include <iostream>
using namespace std;
namespace neuromapp {
    class Timer {
        private:
            chrono::time_point<chrono::system_clock> start_,end_;
            chrono::duration<double,std::milli> duration_;
        public:
            void start(string & message);
            void start();
            void print(string && message);
            double duration();
            void end();
            class Exception {
                private:
                    string message_;
                public:
                    Exception (string & creation_msg ) : message_{creation_msg} {}
                    ~Exception () {};
            };
    };

    /**
     * Timer::start 
     *
     *
     * @brief This function expands on the basic start function by allowing for a message to be printed just before the timer starts running.
     *
     * @param string & message
     *
     * @return void
     */
    void Timer::start(string & message) {
        std::cout << message << std::endl;
        Timer::start();
    }

    /**
     * Timer::start 
     *
     *
     * @brief This function uses the chrono library to start a milisecond timer that will be stopped by the stop function.
     *
     * @param 
     *
     * @return void
     */
    void Timer::start() {
        start_ = chrono::system_clock::now();
    }

    /**
     * Timer::duration 
     *
     *
     * @brief This function calculates the difference in the time between start and stop values. Again, measurement is in miliseconds.
     *
     * @param 
     *
     * @return double
     */
    double Timer::duration() {
        //check to see if duration has been set
        return duration_.count();
    }
    /**
     * Timer::print 
     *
     *
     * @brief Outputs the duration plus an additional message at the front.
     *
     * @param string &&  message
     *
     * @return void
     */
    void Timer::print(string &&  message) {
        std::cout << message << " "<< "time: " << duration_.count()<< "(ms)" << std::endl;
    }

    /**
     * Timer::end 
     *
     *
     * @brief This function is the complement of the "start" function, and populates a class member with the system clock time when the function gets called.
     *
     * @param 
     *
     * @return void
     */
    void Timer::end() {
        end_ = chrono::system_clock::now();
        duration_ = end_-start_;
    }
}
#endif
