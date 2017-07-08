#include <chrono>
#include <iostream>
using namespace std;
namespace neuromapp {
    class Timer {
        private:
            chrono::time_point<chrono::system_clock> start,end;
        public:
            void start(string & message);
            void start();
            void duration(string & message);
            void duration();
    }

    void Timer::start(string & message) {
        std::cout << message << std::endl;
        Timer::start();
    }

    void Timer::start() {
        start = chrono::system_clock::now();
    }

    void Timer::end(string & message) {
        std::cout << message << std::endl;
        Timer::end();
    }

    void Timer::end() {
        end = chrono::system_clock::now();
        chrono::duration<double,std::milli> duration = end-start;
        std::cout << "timed: " << duration.count()<< "(ms)" << std::endl;
    }
}





