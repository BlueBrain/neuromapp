/* Filename : timer_tool.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, Blue Brain Project
 * Purpose : xxx
 * Date : 2017-08-01 
 */
/* Filename : timer_tool.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : xxx
 * Date : 2017-07-20 
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

    void Timer::start(string & message) {
        std::cout << message << std::endl;
        Timer::start();
    }

    void Timer::start() {
        start_ = chrono::system_clock::now();
    }

    double Timer::duration() {
        //check to see if duration has been set
        return duration_.count();
    }
    void Timer::print(string &&  message) {
        std::cout << message << " "<< "time: " << duration_.count()<< "(ms)" << std::endl;
    }

    void Timer::end() {
        end_ = chrono::system_clock::now();
        duration_ = end_-start_;
    }
}
#endif
