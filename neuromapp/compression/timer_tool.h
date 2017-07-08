#include <chrono>
#include <string>
#include <iostream>
using namespace std;
namespace neuromapp {
    class Timer {
        private:
            chrono::time_point<chrono::system_clock> start,end;
            chrono::duration<double,std::milli> duration = -1;
        public:
            void start(string & message);
            void start();
            void duration(string & message);
            void duration();
        class Exception {
            private:
                string _message;
            public:
                Exception (string & creation_msg ) : _message{creation_message} {}
                ~Exception () {};

        };
    };

    void Timer::start(string & message) {
        std::cout << message << std::endl;
        Timer::start();
    }

    void Timer::start() {
        start = chrono::system_clock::now();
    }

    void Timer::duration() {
        //check to see if duration has been set
        if ( duration == -1) throw (Exception("Duration not initialized, missing end()");
        return duration;
    }
178
    void Timer::print(string & message) {
        std::cout << message << " "<< "time: " << duration.count()<< "(ms)" << std::endl;
    }

    void Timer::end() {
        end = chrono::system_clock::now();
        duration = end-start;
    }
}





